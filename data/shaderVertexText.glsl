#ifdef GL_ES
precision mediump float;
#endif

// Input vertex data, different for all executions of this shader.
attribute vec2 positionScreenSpace;
attribute vec2 vertexUV;

// Output data ; will be interpolated for each fragment.
varying vec2 UV;

void main()
{
	// Output position of the vertex, in clip space
	// map [0..800][0..600] to [-1..1][-1..1]
	vec2 positionHomoneuousSpace = positionScreenSpace - vec2(400,300); // [0..800][0..600] -> [-400..400][-300..300]
	positionHomoneuousSpace /= vec2(400,300);
	gl_Position =  vec4(positionHomoneuousSpace, 0, 1);
	
	// UV of the vertex. No special space for this one.
	UV = vertexUV;
}

