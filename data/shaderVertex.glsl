//#version 120

#ifdef GL_ES
precision mediump float;
#endif

attribute vec3 positionModelSpace;
attribute vec3 normalModelSpace;
attribute vec2 vertexUV;

// Output data ; will be interpolated for each fragment.
varying vec2 UV;
varying vec3 positionWorldSpace;
varying vec3 normalCameraSpace;
varying vec3 lightDirectionCameraSpace;
varying vec3 eyeDirectionCameraSpace;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;
uniform mat4 M;
uniform mat4 V;
uniform vec3 lightPositionWorldSpace;

void main(void)
{
	gl_Position =  MVP * vec4(positionModelSpace, 1);

	// Position of the vertex, in worldspace : M * position
	positionWorldSpace = (M * vec4(positionModelSpace, 1)).xyz;

	// Vector that goes from the vertex to the camera, in camera space.
	// In camera space, the camera is at the origin (0,0,0).
	vec3 positionCameraSpace = (V * M * vec4(positionModelSpace, 1)).xyz;
	eyeDirectionCameraSpace = vec3(0,0,0) - positionCameraSpace;

	// Vector that goes from the vertex to the light, in camera space. M is ommited because it's identity.
	vec3 lightPositionCameraSpace = (V * vec4(lightPositionWorldSpace, 1)).xyz;
	lightDirectionCameraSpace = lightPositionCameraSpace + eyeDirectionCameraSpace;

	// Normal of the the vertex, in camera space
	// Only correct if ModelMatrix does not scale the model ! Use its inverse transpose if not.
	normalCameraSpace = (V * M * vec4(normalModelSpace, 0)).xyz;

	UV = vertexUV;
}
