#version 100

#ifdef GL_ES
precision mediump float;
#endif

varying vec3 positionWorldSpace;
varying vec3 normalWorldSpace;
varying vec2 vertexUVA;

struct Light
{
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

// this is material possibly add it to struct ???
uniform sampler2D textureDiffuse0;
uniform sampler2D textureSpecular0;
uniform float shininess;

uniform Light light;
uniform vec3 cameraWorldSpace;

void main(void)
{
    vec3 diffuseColor = texture2D(textureDiffuse0, vertexUVA).rgb;
	vec3 specularColor = texture2D(textureSpecular0, vertexUVA).rgb;

	float distance    = length(light.position - positionWorldSpace);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	// ambient
	vec3 ambient = light.ambient * diffuseColor * attenuation;

    // diffuse
	vec3 ligthDirectionWorldSpace = normalize(light.position - positionWorldSpace);
	float diffuseFactor = max(dot(normalize(normalWorldSpace), ligthDirectionWorldSpace), 0.0);
	vec3 diffuse = light.diffuse * diffuseFactor * diffuseColor * attenuation;

	// specular
	vec3 cameraDirectionWorldSpace = normalize(cameraWorldSpace - positionWorldSpace);
	vec3 reflectDirectionWorldSpace = reflect(-ligthDirectionWorldSpace, normalize(normalWorldSpace));
	float specularFactor = pow(max(dot(cameraDirectionWorldSpace, reflectDirectionWorldSpace), 0.0), shininess);
	vec3 specular = light.specular * specularFactor * specularColor * attenuation;

    vec3 result = ambient + diffuse + specular;

	gl_FragColor.rgb = result;
	gl_FragColor.a = 1.0;
}
