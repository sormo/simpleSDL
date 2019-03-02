#version 100

#ifdef GL_ES
precision mediump float;
#endif

attribute vec3 positionModelSpace;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

void main()
{
    gl_Position = lightSpaceMatrix * model * vec4(positionModelSpace, 1.0);
}
