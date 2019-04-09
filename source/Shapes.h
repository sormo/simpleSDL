#pragma once
#include "OpenGL.h"
#include "ModelShader.h"

namespace Shapes
{
    struct Shape
    {
        Shape(ModelShader & ashader);
        ~Shape();

        void Init(const float * data, size_t dataSize, const uint16_t * indices, size_t indicesSize, size_t count);

        void Bind();
        void Draw();

        GLuint vao;
        GLuint vboData;
        GLuint vboIndices;
        ModelShader & shader;
        size_t count;
    };

    struct Cube : public Shape
    {
        Cube(ModelShader & ashader);
    };

    struct Sphere : public Shape
    {
        Sphere(ModelShader & shader);
    };
}
