#version 100

#ifdef GL_ES
precision mediump float;
#endif

varying vec2 vertexUVA;

uniform sampler2D screenTexture;

void main(void)
{
    vec4 color = texture2D(screenTexture, vertexUVA).rgba;
    gl_FragColor = vec4(color);
}
