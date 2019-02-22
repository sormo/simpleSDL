#version 100

#ifdef GL_ES
precision mediump float;
#endif

varying vec3 positionWorldSpace;
varying vec3 normalWorldSpace;
varying vec2 vertexUVA;

uniform vec3 cameraWorldSpace;
uniform samplerCube textureSkybox;

void main()
{             
    float ratio = 1.00 / 1.52;

    vec3 I = normalize(positionWorldSpace - cameraWorldSpace);
    vec3 R = refract(I, normalize(normalWorldSpace), ratio);
    
    gl_FragColor = vec4(textureCube(textureSkybox, R).rgb, 1.0);
}
