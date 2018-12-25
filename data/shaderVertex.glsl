//#version 120

#ifdef GL_ES
precision mediump float;
#endif

attribute vec3 position;
attribute vec2 vertexUV;

// Output data ; will be interpolated for each fragment.
varying vec2 UV;
// Values that stay constant for the whole mesh.
uniform mat4 MVP;

void main(void)
{
  gl_Position =  MVP * vec4(position,1);

  UV = vertexUV;
}
