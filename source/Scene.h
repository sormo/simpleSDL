#pragma once
#include "ModelShader.h"
#include <memory>
#include <vector>
#include "Bullet.h"

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

    void AddCube(const glm::vec3 & position, const glm::vec3 & rotation, const glm::vec3 & scale, const Material::Data & material, bool isStatic);
    void PopCube();

    void Step();

    void Draw(const glm::mat4 & view, const glm::mat4 & projection, const glm::vec3 & cameraPosition, const Light::Data & data);
    void DrawDebug(const glm::mat4 & view, const glm::mat4 & projection);

private:
    std::unique_ptr<ModelShader> m_shader;

    struct CubeData
    {
        glm::mat4 model;
        glm::vec3 scale;
        Material::Data material;
        btRigidBody * body;
    };

    std::vector<CubeData> m_cubes;

    void RefreshCubeModels();
    void RefreshCubeModel(CubeData & cube);

    std::unique_ptr<Shape::Cube> m_cube;

    Bullet m_world;
};
