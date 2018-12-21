//#version 120

#ifdef GL_ES
precision mediump float;
#endif

varying vec3 outputColor;

void main(void)
{
    gl_FragColor.xyz = outputColor;
    gl_FragColor.w = 1.0;
}
