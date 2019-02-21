#version 100

#ifdef GL_ES
precision mediump float;
#endif

varying vec3 vertexUVA;
uniform samplerCube skybox;

void main()
{    
    gl_FragColor = textureCube(skybox, vertexUVA);
}
