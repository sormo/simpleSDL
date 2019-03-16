#version 100

#ifdef GL_ES
precision mediump float;
#endif

varying vec3 positionWorldSpace;
varying vec4 positionLightSpace;
varying vec3 normalWorldSpace;
varying vec2 vertexUVA;

struct Light
{
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform sampler2D textureDiffuse;
uniform sampler2D depthMap;
uniform float shininess;
uniform Light light;
uniform vec3 cameraWorldSpace;
uniform vec2 depthMapSize;

float ShadowCalculation(vec4 position, vec3 normal, vec3 lightDirection)
{
    // perform perspective divide, result is in [-1, 1]
    vec3 projCoords = position.xyz / position.w;
    // tansform to range [0, 1]
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range positionLightSpace as coords)
    float closestDepth = texture2D(depthMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

    // check whether current frag pos is in shadow
    // bias to prevent shadow acne
	float bias = max(0.05 * (1.0 - dot(normal, lightDirection)), 0.005);

    // check whether current frag pos is in shadow
    // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / depthMapSize;
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture2D(depthMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;

    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;

    return shadow;
}

void main(void)
{
    vec3 diffuseColor = texture2D(textureDiffuse, vertexUVA).rgb;
	vec3 lightColor = vec3(1.0, 1.0, 1.0);

	// ambient
	vec3 ambient = light.ambient * diffuseColor;

    // diffuse
	vec3 ligthDirectionWorldSpace = normalize(light.position - positionWorldSpace);
	float diffuseFactor = max(dot(normalize(normalWorldSpace), ligthDirectionWorldSpace), 0.0);
	vec3 diffuse = light.diffuse * diffuseFactor * diffuseColor * lightColor;

	// specular
	vec3 cameraDirectionWorldSpace = normalize(cameraWorldSpace - positionWorldSpace);
	//vec3 reflectDirectionWorldSpace = reflect(-ligthDirectionWorldSpace, normalize(normalWorldSpace));
	vec3 halfwayDirectionWorldSpace = normalize(ligthDirectionWorldSpace + cameraDirectionWorldSpace);

	float specularFactor = pow(max(dot(cameraDirectionWorldSpace, halfwayDirectionWorldSpace), 0.0), shininess);
	vec3 specular = light.specular * specularFactor * lightColor;

	float shadow = ShadowCalculation(positionLightSpace, normalWorldSpace, ligthDirectionWorldSpace);

    vec3 result = ambient + (diffuse + specular)*(1.0 - shadow);

	gl_FragColor.rgb = result;
	gl_FragColor.a = 1.0;
}
