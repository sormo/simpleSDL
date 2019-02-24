#version 100

#ifdef GL_ES
precision mediump float;
#endif

attribute vec2 position;
attribute vec2 vertexUV;

varying vec2 vertexUVA;

void main(void)
{
	gl_Position =  vec4(position.x, position.y, 0.0, 1.0); 
	vertexUVA = vertexUV;
}
