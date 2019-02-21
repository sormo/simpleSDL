#version 100

#ifdef GL_ES
precision mediump float;
#endif

varying vec3 positionWorldSpace;
varying vec2 vertexUVA;
uniform sampler2D textureDiffuse;

void main(void)
{
    vec3 diffuseColor = texture2D(textureDiffuse, vertexUVA).rgb;

	gl_FragColor.rgb = diffuseColor;
	gl_FragColor.a = 1.0;
}
