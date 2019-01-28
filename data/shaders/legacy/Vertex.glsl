#version 100

#ifdef GL_ES
precision mediump float;
#endif

attribute vec3 positionModelSpace;
attribute vec3 normalModelSpace;
attribute vec3 tangentModelSpace;
attribute vec3 bitangentModelSpace;
attribute vec2 vertexUV;

// Output data ; will be interpolated for each fragment.
varying vec2 UV;
varying vec3 positionWorldSpace;
varying vec3 normalCameraSpace;
varying vec3 lightDirectionCameraSpace;
varying vec3 eyeDirectionCameraSpace;
varying vec3 lightDirectionTangentSpace;
varying vec3 eyeDirectionTangentSpace;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;
uniform mat3 MV3x3;
uniform mat4 M;
uniform mat4 V;
uniform vec3 lightPositionWorldSpace;

mat3 transposeCustom(mat3 inMatrix)
{
    vec3 i0 = inMatrix[0];
    vec3 i1 = inMatrix[1];
    vec3 i2 = inMatrix[2];

    mat3 outMatrix = mat3(
                 vec3(i0.x, i1.x, i2.x),
                 vec3(i0.y, i1.y, i2.y),
                 vec3(i0.z, i1.z, i2.z)
                 );

    return outMatrix;
}

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

	// model to camera = ModelView
	vec3 tangentCameraSpace = MV3x3 * tangentModelSpace;
	vec3 bitangentCameraSpace = MV3x3 * bitangentModelSpace;
	//vec3 normalCameraSpace = MV3x3 * normalModelSpace;
	
	mat3 TBN = transposeCustom(mat3(
		normalize(tangentCameraSpace),
		normalize(bitangentCameraSpace),
		normalize(normalCameraSpace)	
	)); // You can use dot products instead of building this matrix and transposing it. See References for details.

	lightDirectionTangentSpace = TBN * lightDirectionCameraSpace;
	eyeDirectionTangentSpace =  TBN * eyeDirectionCameraSpace;
}
