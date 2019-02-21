#version 100

#ifdef GL_ES
precision mediump float;
#endif

attribute vec3 positionModelSpace;
attribute vec2 vertexUV;

uniform mat4 MVP;
varying vec2 vertexUVA;

void main(void)
{
	gl_Position =  MVP * vec4(positionModelSpace, 1.0);
	
	vertexUVA = vertexUV;
}
