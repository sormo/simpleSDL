#version 140

#ifdef GL_ES
precision mediump float;
#endif

attribute vec3 positionModelSpace;
attribute vec3 normalModelSpace;
attribute vec2 vertexUV;
attribute vec3 tangentModelSpace;
attribute vec3 bitangentModelSpace;

uniform mat4 MVP;
uniform mat4 M;
uniform vec3 lightWorldSpace;
uniform vec3 cameraWorldSpace;

varying vec2 vertexUVA;
varying vec3 positionWorldSpace;
varying vec3 lightTangentSpace;
varying vec3 cameraTangentSpace;
varying vec3 positionTangentSpace;

void main(void)
{
	gl_Position =  MVP * vec4(positionModelSpace, 1.0);
	
	positionWorldSpace = vec3(M * vec4(positionModelSpace, 1.0));

	mat3 normalMatrix = transpose(inverse(mat3(M)));
	vec3 T = normalize(normalMatrix * tangentModelSpace);
    vec3 N = normalize(normalMatrix * normalModelSpace);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    mat3 TBN = transpose(mat3(T, B, N));

   	lightTangentSpace = TBN * lightWorldSpace;
    cameraTangentSpace  = TBN * cameraWorldSpace;
    positionTangentSpace  = TBN * positionWorldSpace;

	vertexUVA = vertexUV;
}
