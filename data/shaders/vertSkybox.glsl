#version 100

#ifdef GL_ES
precision mediump float;
#endif

attribute vec3 position;
varying vec3 vertexUVA;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    vertexUVA = position;

	vec4 resultPosition = projection * view * vec4(position, 1.0);

    gl_Position = resultPosition.xyww;
}
