#version 100

#ifdef GL_ES
precision mediump float;
#endif

varying vec2 vertexUVA;

uniform sampler2D colorMap;
uniform float nearPlane;
uniform float farPlane;

// required when using a perspective projection matrix
float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * nearPlane * farPlane) / (farPlane + nearPlane - z * (farPlane - nearPlane));	
}

void main()
{             
    float depthValue = texture2D(colorMap, vertexUVA).r;
    // gl_FragColor = vec4(vec3(LinearizeDepth(depthValue) / farPlane), 1.0); // perspective
    gl_FragColor = vec4(vec3(depthValue), 1.0); // orthographic
}
