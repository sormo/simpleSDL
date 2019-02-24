#version 100

#ifdef GL_ES
precision mediump float;
#endif

varying vec2 vertexUVA;

uniform sampler2D screenTexture;

void main(void)
{
    gl_FragColor = vec4(vec3(1.0 - texture2D(screenTexture, vertexUVA)), 1.0);
}
