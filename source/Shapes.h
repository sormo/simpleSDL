#pragma once
#include "OpenGL.h"
#include "ModelShader.h"
#include <variant>

namespace Shapes
{
    // this does not belong much here
    // define shape in world
    namespace Defintion
    {
        struct Box
        {
            glm::vec3 position;
            glm::vec3 rotation;
            glm::vec3 extents;
        };

        struct Sphere
        {
            glm::vec3 position;
            glm::vec3 rotation;
            float radius;
        };

        struct Cylinder
        {
            glm::vec3 position;
            glm::vec3 rotation;
            float radius;
            float height;
        };

        struct Cone
        {
            glm::vec3 position;
            glm::vec3 rotation;
            float radius;
            float height;
        };
    }

    enum class Type { Cube, Sphere, Cylinder, Cone };

    struct FreeShader
    {
        Shader * shader;
        GLuint locationPosition = -1;
        GLuint locationNormal = -1;
        GLuint locationUV = -1;
    };
    using ShapeShader = std::variant<ModelShader*, FreeShader*>;

    struct Shape
    {
        Shape(ShapeShader ashader, Type atype);
        ~Shape();

        void Init(const float * data, size_t dataSize, const uint16_t * indices, size_t indicesSize, size_t count);

        void Bind();
        void Draw();

        GLuint vao;
        GLuint vboData;
        GLuint vboIndices;
        ShapeShader shader;
        size_t count;
        Type type;
    };

    struct Cube : public Shape
    {
        Cube(ShapeShader ashader);
    };

    struct Sphere : public Shape
    {
        Sphere(ShapeShader shader);
    };

    struct Cylinder : public Shape
    {
        Cylinder(ShapeShader shader);
    };

    struct Cone : public Shape
    {
        Cone(ShapeShader shader);
    };
}
