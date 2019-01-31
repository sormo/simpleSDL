#version 100

#ifdef GL_ES
precision mediump float;
#endif

attribute vec3 positionModelSpace;
attribute vec3 normalModelSpace;

uniform mat4 MVP;
uniform mat4 M;
uniform vec3 lightWorldSpace;

varying vec3 positionWorldSpace;
varying vec3 normalWorldSpace;

void main(void)
{
	gl_Position =  MVP * vec4(positionModelSpace, 1.0);
	
	positionWorldSpace = vec3(M * vec4(positionModelSpace, 1.0));
	normalWorldSpace = normalModelSpace;
}
