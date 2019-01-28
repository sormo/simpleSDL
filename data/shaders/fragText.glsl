#ifdef GL_ES
precision mediump float;
#endif

// Interpolated values from the vertex shaders
varying vec2 UV;

// Values that stay constant for the whole mesh.
uniform sampler2D textureValue;

void main()
{
	gl_FragColor = texture2D(textureValue, UV);
}
