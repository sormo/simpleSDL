#version 100

#ifdef GL_ES
precision mediump float;
#endif

varying vec3 positionWorldSpace;
varying vec3 normalWorldSpace;
varying vec2 vertexUVA;
varying mat3 TBN;

struct Light
{
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

// this is material possibly add it to struct ???
uniform sampler2D textureDiffuse;
uniform sampler2D textureNormal;
uniform sampler2D textureSpecular;
uniform float shininess;

uniform Light light;
uniform vec3 cameraWorldSpace;

void main(void)
{
    vec3 diffuseColor = texture2D(textureDiffuse, vertexUVA).rgb;
    vec3 specularColor = texture2D(textureSpecular, vertexUVA).rgb;

    vec3 normalTangentSpace = texture2D(textureNormal, vertexUVA).rgb;
    normalTangentSpace = normalize(normalTangentSpace * 2.0 - 1.0);   
    vec3 normal = normalize(TBN * normalTangentSpace);
    //vec3 normal = normalWorldSpace;

	// ambient
	vec3 ambient = light.ambient * diffuseColor;

    // diffuse
	vec3 ligthDirectionWorldSpace = normalize(light.position - positionWorldSpace);
	float diffuseFactor = max(dot(normal, ligthDirectionWorldSpace), 0.0);
	vec3 diffuse = light.diffuse * diffuseFactor * diffuseColor;

	// specular
	vec3 cameraDirectionWorldSpace = normalize(cameraWorldSpace - positionWorldSpace);
	vec3 reflectDirectionWorldSpace = reflect(-ligthDirectionWorldSpace, normal);
	float specularFactor = pow(max(dot(cameraDirectionWorldSpace, reflectDirectionWorldSpace), 0.0), shininess);
	vec3 specular = light.specular * specularFactor * specularColor;

    // spotlight (soft edges)
    float theta = dot(ligthDirectionWorldSpace, normalize(-light.direction)); 
    float epsilon = (light.cutOff - light.outerCutOff);
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    diffuse  *= intensity;
    //specular *= intensity;
    
    // attenuation
    float distance    = length(light.position - positionWorldSpace);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    
    ambient  *= attenuation; 
    diffuse  *= attenuation;
    //specular *= attenuation;

    vec3 result = ambient + diffuse + specular;

	gl_FragColor.rgb = result;
	gl_FragColor.a = 1.0;
}
