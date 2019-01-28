#version 100

#ifdef GL_ES
precision mediump float;
#endif

attribute vec3 positionModelSpace;
uniform mat4 MVP;

void main(void)
{
	gl_Position =  MVP * vec4(positionModelSpace, 1.0);
}
