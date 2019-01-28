#version 100

#ifdef GL_ES
precision mediump float;
#endif

attribute vec3 positionModelSpace;
attribute vec2 vertexUV;
varying vec2 UV;
uniform mat4 MVP;

void main(void)
{
	gl_Position =  MVP * vec4(positionModelSpace, 1.0);
	
	UV = vertexUV;
}
