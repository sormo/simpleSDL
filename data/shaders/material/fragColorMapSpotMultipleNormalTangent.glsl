#version 100

#ifdef GL_ES
precision mediump float;
#endif

#define TEXTURE_DIFFUSE_COUNT 1
#define TEXTURE_SPECULAR_COUNT 1
#define TEXTURE_NORMAL_COUNT 1
#define LIGHT_POINT_COUNT 1
#define LIGHT_SPOT_COUNT 1

varying vec3 positionWorldSpace;
varying vec3 normalWorldSpace;
varying vec2 vertexUVA;

struct LightDirectional
{
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct LightPoint
{
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

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
uniform sampler2D textureNormal[TEXTURE_NORMAL_COUNT];
uniform float shininess;

uniform Material material;
uniform LightDirectional lightDirectional;
uniform LightPoint lightPoint[LIGHT_POINT_COUNT];
uniform LightSpot lightSpot[LIGHT_SPOT_COUNT];
uniform vec3 cameraWorldSpace;

varying vec3 positionTangentSpace;
varying vec3 cameraTangentSpace;

varying vec3 lighPointPositionTangentSpace[LIGHT_POINT_COUNT];
varying vec3 lighSpotPositionTangentSpace[LIGHT_SPOT_COUNT];

// function prototypes
vec3 CalcLightDirectional(LightDirectional light, vec3 ambientColor, vec3 diffuseColor, vec3 specularColor, vec3 cameraDirectionTangentSpace, vec3 normal);
vec3 CalcLightPoint(LightPoint light, vec3 ambientColor, vec3 diffuseColor, vec3 specularColor, vec3 cameraDirectionTangentSpace, vec3 normal);
vec3 CalcLightSpot(LightSpot light, vec3 lightPositionTangentSpace, vec3 ambientColor, vec3 diffuseColor, vec3 specularColor, vec3 cameraDirectionTangentSpace, vec3 normal);

void main(void)
{
    vec3 ambientColor = material.ambient;
    vec3 diffuseColor = texture2D(textureDiffuse[0], vertexUVA).rgb;
	vec3 specularColor = material.specular;

    vec3 cameraDirectionTangentSpace = normalize(cameraTangentSpace - positionTangentSpace);

    vec3 normalTangentSpace = texture2D(textureNormal[0], vertexUVA).rgb;
    normalTangentSpace = normalize(normalTangentSpace * 2.0 - 1.0);   

    vec3 result = vec3(0.0, 0.0, 0.0);

    //vec3 result = CalcLightDirectional(lightDirectional, ambientColor, diffuseColor, specularColor, cameraDirectionTangentSpace, normal);

    //for(int i = 0; i < LIGHT_POINT_COUNT; i++)
    //    result += CalcLightPoint(lightPoint[i], ambientColor, diffuseColor, specularColor, cameraDirectionTangentSpace, normal);

    for(int i = 0; i < LIGHT_SPOT_COUNT; i++)
        result += CalcLightSpot(lightSpot[i], lighSpotPositionTangentSpace[i], ambientColor, diffuseColor, specularColor, cameraDirectionTangentSpace, normalTangentSpace);

	gl_FragColor.rgb = result;
	gl_FragColor.a = 1.0;
}

// calculates the color when using a directional light.
vec3 CalcLightDirectional(LightDirectional light, vec3 ambientColor, vec3 diffuseColor, vec3 specularColor, vec3 cameraDirectionTangentSpace, vec3 normal)
{
    vec3 ligthDirection = normalize(-light.direction);
    // diffuse shading
    float diffuseFactor = max(dot(normalize(normal), ligthDirection), 0.0);
    // specular shading
    vec3 reflectDirection = reflect(-ligthDirection, normalize(normal));
    float specularFactor = pow(max(dot(cameraDirectionTangentSpace, reflectDirection), 0.0), shininess);
    // combine results
    vec3 ambient = light.ambient * ambientColor;
    vec3 diffuse = light.diffuse * diffuseFactor * diffuseColor;
    vec3 specular = light.specular * specularFactor * specularColor;

    return (ambient + diffuse + specular);
}

// calculates the color when using a point light.
vec3 CalcLightPoint(LightPoint light, vec3 lightPositionTangentSpace, vec3 ambientColor, vec3 diffuseColor, vec3 specularColor, vec3 cameraDirectionTangentSpace, vec3 normal)
{
    vec3 ligthDirectionTangentSpace = normalize(lightPositionTangentSpace - positionTangentSpace);
    // diffuse shading
    float diffuseFactor = max(dot(normal, ligthDirectionTangentSpace), 0.0);
    // specular shading
    vec3 reflectDirectionTangentSpace = reflect(-ligthDirectionTangentSpace, normal);
    float specularFactor = pow(max(dot(cameraDirectionTangentSpace, reflectDirectionTangentSpace), 0.0), shininess);
    // attenuation
    float distance = length(light.position - positionWorldSpace);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient = light.ambient * ambientColor;
    vec3 diffuse = light.diffuse * diffuseFactor * diffuseColor;
    vec3 specular = light.specular * specularFactor * specularColor;
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}

// calculates the color when using a spot light.
vec3 CalcLightSpot(LightSpot light, vec3 lightPositionTangentSpace, vec3 ambientColor, vec3 diffuseColor, vec3 specularColor, vec3 cameraDirectionTangentSpace, vec3 normal)
{
    vec3 ligthDirectionTangentSpace = normalize(lightPositionTangentSpace - positionTangentSpace);
    // diffuse shading
    float diffuseFactor = max(dot(normal, ligthDirectionTangentSpace), 0.0);
    // specular shading
    vec3 reflectDirectionTangentSpace = reflect(-ligthDirectionTangentSpace, normal);
    float specularFactor = pow(max(dot(cameraDirectionTangentSpace, reflectDirectionTangentSpace), 0.0), shininess);
    // attenuation
    float distance = length(light.position - positionWorldSpace);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // spotlight intensity
    float theta = dot(ligthDirectionTangentSpace, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
    vec3 ambient = light.ambient * ambientColor;
    vec3 diffuse = light.diffuse * diffuseFactor * diffuseColor;
    vec3 specular = light.specular * specularFactor * specularColor;
    ambient *= attenuation * intensity;;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    return (ambient + diffuse + specular);
}
