//#version 120

#ifdef GL_ES
precision mediump float;
#endif

varying vec2 UV;
uniform sampler2D textureValue;

void main(void)
{
    gl_FragColor.xyz = texture2D(textureValue, UV).rgb;
    gl_FragColor.w = 1.0;
}
