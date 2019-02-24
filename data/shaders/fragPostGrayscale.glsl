#version 100

#ifdef GL_ES
precision mediump float;
#endif

varying vec2 vertexUVA;

uniform sampler2D screenTexture;

void main(void)
{
    vec4 color = texture2D(screenTexture, vertexUVA);
    float average = 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;

    gl_FragColor = vec4(average, average, average, 1.0);
}
