#version 100

#ifdef GL_ES
precision mediump float;
#endif

varying vec2 vertexUVA;

uniform sampler2D screenTexture;
uniform float kernel[9];

const float offset = 1.0 / 300.0;  

void main()
{
    vec2 offsets[9];

    offsets[0] = vec2(-offset, offset);  // top-left
    offsets[1] = vec2( 0.0,    offset);  // top-center
    offsets[2] = vec2( offset, offset);  // top-right
    offsets[3] = vec2(-offset, 0.0);     // center-left
    offsets[4] = vec2( 0.0,    0.0);     // center-center
    offsets[5] = vec2( offset, 0.0);     // center-right
    offsets[6] = vec2(-offset, -offset); // bottom-left
    offsets[7] = vec2( 0.0,    -offset); // bottom-center
    offsets[8] = vec2( offset, -offset); // bottom-right    
   
    vec3 sampleTex[9];
    for(int i = 0; i < 9; i++)
        sampleTex[i] = vec3(texture2D(screenTexture, vertexUVA.st + offsets[i]));

    vec3 color = vec3(0.0);
    for(int i = 0; i < 9; i++)
        color += sampleTex[i] * kernel[i];
    
    gl_FragColor = vec4(color, 1.0);
}
