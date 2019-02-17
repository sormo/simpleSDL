#version 100

#ifdef GL_ES
precision mediump float;
#endif

#define TEXTURE_DIFFUSE_COUNT 1
#define TEXTURE_SPECULAR_COUNT 1
#define LIGHT_SPOT_COUNT 1

varying vec3 positionWorldSpace;
varying vec3 normalWorldSpace;
varying vec2 vertexUVA;

struct LightSpot
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

struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform sampler2D textureDiffuse[TEXTURE_DIFFUSE_COUNT];
uniform sampler2D textureSpecular[TEXTURE_SPECULAR_COUNT];
uniform float shininess;

uniform Material material;
uniform LightSpot lightSpot[LIGHT_SPOT_COUNT];
uniform vec3 cameraWorldSpace;

void main(void)
{
    vec3 diffuseColor = texture2D(textureDiffuse[0], vertexUVA).rgb;
	vec3 specularColor = texture2D(textureSpecular[0], vertexUVA).rgb;

	// ambient
	vec3 ambient = lightSpot[0].ambient * material.ambient;

    // diffuse
	vec3 ligthDirectionWorldSpace = normalize(lightSpot[0].position - positionWorldSpace);
	float diffuseFactor = max(dot(normalize(normalWorldSpace), ligthDirectionWorldSpace), 0.0);
	vec3 diffuse = lightSpot[0].diffuse * diffuseFactor * diffuseColor;

	// specular
	vec3 cameraDirectionWorldSpace = normalize(cameraWorldSpace - positionWorldSpace);
	vec3 reflectDirectionWorldSpace = reflect(-ligthDirectionWorldSpace, normalize(normalWorldSpace));
	float specularFactor = pow(max(dot(cameraDirectionWorldSpace, reflectDirectionWorldSpace), 0.0), shininess);
	vec3 specular = lightSpot[0].specular * specularFactor * specularColor;

    // spotlight (soft edges)
    float theta = dot(ligthDirectionWorldSpace, normalize(-lightSpot[0].direction)); 
    float epsilon = (lightSpot[0].cutOff - lightSpot[0].outerCutOff);
    float intensity = clamp((theta - lightSpot[0].outerCutOff) / epsilon, 0.0, 1.0);
    diffuse  *= intensity;
    specular *= intensity;
    
    // attenuation
    float distance    = length(lightSpot[0].position - positionWorldSpace);
    float attenuation = 1.0 / (lightSpot[0].constant + lightSpot[0].linear * distance + lightSpot[0].quadratic * (distance * distance));    
    ambient  *= attenuation; 
    diffuse  *= attenuation;
    specular *= attenuation;

    vec3 result = ambient + diffuse + specular;

	gl_FragColor.rgb = result;
	gl_FragColor.a = 1.0;
}
