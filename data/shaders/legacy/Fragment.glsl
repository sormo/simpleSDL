#version 100

#ifdef GL_ES
precision mediump float;
#endif

varying vec2 UV;
varying vec3 positionWorldSpace;
varying vec3 normalCameraSpace;
varying vec3 lightDirectionCameraSpace;
varying vec3 eyeDirectionCameraSpace;
varying vec3 lightDirectionTangentSpace;
varying vec3 eyeDirectionTangentSpace;

uniform sampler2D textureDiffuse;
uniform sampler2D textureNormal;
uniform vec3 lightPositionWorldSpace;

void main(void)
{
	// Local normal, in tangent space
	vec3 normalTangentSpace = texture2D(textureNormal, UV).rgb;
    // transform normal vector to range [-1,1]
    // this normal is in tangent space
    normalTangentSpace = normalize(normalTangentSpace * 2.0 - 1.0);

    // get color from texture
	vec3 diffuseColor = texture2D(textureDiffuse, UV).rgb;

	// ambient color
	vec3 ambientPart = 0.1 * diffuseColor;

	// diffuse color
	vec3 lightDirection = normalize(lightDirectionTangentSpace);
	vec3 normal = normalize(normalTangentSpace);
	float diffuseFactor = max(dot(lightDirection, normal), 0.0);
	vec3 diffusePart = diffuseColor * diffuseFactor;

	// specular color
	vec3 viewDirection = normalize(eyeDirectionTangentSpace);
	vec3 reflectDirection = reflect(-lightDirection, normal);
	vec3 halfwayDirection = normalize(lightDirection + viewDirection);
	float specularFactor = pow(max(dot(normal, halfwayDirection), 0.0), 32.0);
	vec3 specularPart = vec3(0.2) * specularFactor;

	// Distance to the light
	//float distance = length(lightPositionWorldSpace - positionWorldSpace);

	gl_FragColor.rgb =
		// Ambient : simulates indirect lighting 
		ambientPart +
		// Diffuse : "color" of the object
		diffusePart +
		// Specular : reflective highlight, like a mirror
    	specularPart;
    gl_FragColor.a = 1.0;
}
