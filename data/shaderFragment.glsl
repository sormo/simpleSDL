//#version 120

#ifdef GL_ES
precision mediump float;
#endif

varying vec2 UV;
varying vec3 positionWorldSpace;
varying vec3 normalCameraSpace;
varying vec3 lightDirectionCameraSpace;
varying vec3 eyeDirectionCameraSpace;

uniform sampler2D textureValue;
uniform vec3 lightPositionWorldSpace;

void main(void)
{
	// Light emission properties
	// You probably want to put them as uniforms
	vec3 lightColor = vec3(1, 1, 1);
	float lightPower = 50.0;

	vec3 diffuseColor = texture2D(textureValue, UV).rgb;
	vec3 ambientColor = vec3(0.05, 0.05, 0.05) * diffuseColor;
	vec3 specularColor = vec3(0.3, 0.3, 0.3);

	// Distance to the light
	float distance = length(lightPositionWorldSpace - positionWorldSpace);

	// Normal of the computed fragment, in camera space
	vec3 n = normalize(normalCameraSpace);
	// Direction of the light (from the fragment to the light)
	vec3 l = normalize(lightDirectionCameraSpace);
	// Cosine of the angle between the normal and the light direction, 
	// clamped above 0
	//  - light is at the vertical of the triangle -> 1
	//  - light is perpendicular to the triangle -> 0
	//  - light is behind the triangle -> 0
	float cosTheta = clamp(dot(n, l), 0.0, 1.0);

	// Eye vector (towards the camera)
	vec3 E = normalize(eyeDirectionCameraSpace);
	// Direction in which the triangle reflects the light
	vec3 R = reflect(-l, n);
	// Cosine of the angle between the Eye vector and the Reflect vector,
	// clamped to 0
	//  - Looking into the reflection -> 1
	//  - Looking elsewhere -> < 1
	float cosAlpha = clamp(dot(E, R), 0.0, 1.0);

	gl_FragColor.rgb =
		// Ambient : simulates indirect lighting 
		ambientColor +
		// Diffuse : "color" of the object
		diffuseColor * lightColor * lightPower * cosTheta / (distance * distance) + 
		// Specular : reflective highlight, like a mirror
    	specularColor * lightColor * lightPower * pow(cosAlpha, 5.0) / (distance * distance);
    gl_FragColor.a = 0.3;
}
