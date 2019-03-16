#version 100

#ifdef GL_ES
precision mediump float;
#endif

attribute vec3 positionModelSpace;
attribute vec3 normalModelSpace;
attribute vec2 vertexUV;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 lightSpaceMatrix;

varying vec3 positionWorldSpace;
varying vec4 positionLightSpace;
varying vec3 normalWorldSpace;
varying vec2 vertexUVA;

mat3 transposeCustom(mat3 m)
{
    vec3 i0 = m[0];
    vec3 i1 = m[1];
    vec3 i2 = m[2];

    return mat3(vec3(i0.x, i1.x, i2.x),
                vec3(i0.y, i1.y, i2.y),
                vec3(i0.z, i1.z, i2.z));
}

mat3 inverseCustom(mat3 m)
{
    float a00 = m[0][0], a01 = m[0][1], a02 = m[0][2];
    float a10 = m[1][0], a11 = m[1][1], a12 = m[1][2];
    float a20 = m[2][0], a21 = m[2][1], a22 = m[2][2];

    float b01 = a22 * a11 - a12 * a21;
    float b11 = -a22 * a10 + a12 * a20;
    float b21 = a21 * a10 - a11 * a20;

    float det = a00 * b01 + a01 * b11 + a02 * b21;

    return mat3(b01, (-a22 * a01 + a02 * a21), ( a12 * a01 - a02 * a11),
                b11, ( a22 * a00 - a02 * a20), (-a12 * a00 + a02 * a10),
                b21, (-a21 * a00 + a01 * a20), ( a11 * a00 - a01 * a10)) / det;
}

void main(void)
{
    // output position in camera space
    gl_Position = projection * view * model * vec4(positionModelSpace, 1.0);

    positionWorldSpace = vec3(model * vec4(positionModelSpace, 1.0));
    positionLightSpace = lightSpaceMatrix * vec4(positionWorldSpace, 1.0);
    // normal for light
    normalWorldSpace = transposeCustom(inverseCustom(mat3(model))) * normalModelSpace;

    vertexUVA = vertexUV;
}
