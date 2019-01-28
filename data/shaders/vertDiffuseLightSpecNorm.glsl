#version 100

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

varying vec2 vertexUVA;
varying vec3 positionWorldSpace;
varying vec3 lightWorldSpaceA;
varying vec3 normalWorldSpace;
varying mat3 TBN;

void main(void)
{
	gl_Position =  MVP * vec4(positionModelSpace, 1.0);
	
	lightWorldSpaceA = lightWorldSpace;
	positionWorldSpace = vec3(M * vec4(positionModelSpace, 1.0));

	vec3 T = normalize(vec3(M * vec4(tangentModelSpace,   0.0)));
	vec3 B = normalize(vec3(M * vec4(bitangentModelSpace, 0.0)));
	vec3 N = normalize(vec3(M * vec4(normalModelSpace,    0.0)));
	TBN = mat3(T, B, N);

	vertexUVA = vertexUV;
}
