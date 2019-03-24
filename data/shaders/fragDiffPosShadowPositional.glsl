#version 150

#ifdef GL_ES
precision mediump float;
#endif

#extension GL_NV_shadow_samplers_cube : enable

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
uniform float shininess;
uniform Light light;

uniform samplerCube depthMap;
uniform vec2 depthMapSize;

uniform vec3 cameraWorldSpace;
uniform float farPlane;

vec3 SAMPLE_OFFSET_DIRECTIONS[20] = vec3[]
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);  

float ApplyPCF(float currentDepth, vec3 fragmentToLightWorldSpace)
{
    float shadow = 0.0;
    float bias   = 0.15;
    int samples  = 20;
    float viewDistance = length(cameraWorldSpace - positionWorldSpace);
    float diskRadius = (1.0 + (viewDistance / farPlane)) / 25.0;

    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = textureCube(depthMap, fragmentToLightWorldSpace + SAMPLE_OFFSET_DIRECTIONS[i] * diskRadius).r;
        closestDepth *= farPlane; // Undo mapping [0;1]
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }

    shadow /= float(samples);

    return shadow;
}

float ShadowCalculation()
{
    // get vector from fragment to light
    vec3 fragmentToLightWorldSpace = positionWorldSpace - light.position;
    // use the light to fragment vector to sample from the depth map    
    float closestDepth = textureCube(depthMap, fragmentToLightWorldSpace).r;
    // it is currently in linear range between [0,1]. Re-transform back to original value
    closestDepth *= farPlane;
    // now get current linear depth as the length between the fragment and light position
    float currentDepth = length(fragmentToLightWorldSpace);
    // now test for shadows
    //float bias = 0.05; 
    //float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
    float shadow = ApplyPCF(currentDepth, fragmentToLightWorldSpace);

    return shadow;
    //return closestDepth;
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

	float shadow = ShadowCalculation();

    //vec3 result = vec3(shadow / farPlane);  
    vec3 result = ambient + (diffuse + specular)*(1.0 - shadow);
    //vec3 result = ambient + diffuse + specular;

	gl_FragColor.rgb = result;
	gl_FragColor.a = 1.0;
}
