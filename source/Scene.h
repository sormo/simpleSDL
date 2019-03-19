#pragma once
#include "ModelShader.h"
#include <memory>
#include <vector>

namespace Shape
{
    struct Cube
    {
        GLuint vao;
        GLuint vbo;
        ModelShader & shader;

        Cube(ModelShader & ashader);
        ~Cube();

        void Bind();
        void Draw();
    };
}

class Scene
{
public:
    Scene(const Light::Config & light);

    void AddCube(const glm::mat4 & model, const Material::Data & material);

    void Draw(const glm::mat4 & view, const glm::mat4 & projection, const glm::vec3 & cameraPosition, const Light::Data & data);

private:
    std::unique_ptr<ModelShader> m_shader;
    std::vector<std::tuple<glm::mat4, Material::Data>> m_cubes;

    std::unique_ptr<Shape::Cube> m_cube;
};
