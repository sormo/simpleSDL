#pragma once
#include <memory>
#include <map>
#include "Bullet.h"
#include "Shapes.h"
#include "ModelShader.h"

struct SceneShapeHandle;

class Scene
{
    friend struct SceneShapeHandle;
public:
    Scene(const Light::Config & light);
    // desctructor must be implemented  where SceneShapeHandle is defined
    ~Scene();

    using Shape = SceneShapeHandle * ;

    Shape AddCube(const glm::vec3 & position, const glm::vec3 & rotation, const glm::vec3 & scale, const Material::Data & material, bool isStatic);
    Shape AddSphere(const glm::vec3 & position, float radius, const Material::Data & material, bool isStatic);
    void RemoveShape(Shape shape);

    void Step();

    void Draw(const glm::mat4 & view, const glm::mat4 & projection, const glm::vec3 & cameraPosition, const Light::Data & data);
    void DrawDebug(const glm::mat4 & view, const glm::mat4 & projection);

private:
    std::unique_ptr<ModelShader> m_shader;

    struct ShapeData
    {
        glm::mat4 model;
        glm::vec3 scale;
        Material::Data material;
        btRigidBody * body;
        std::unique_ptr<SceneShapeHandle> handle;
    };

    std::multimap<Shapes::Shape *, ShapeData> m_shapes;

    enum class DrawType{ Shadow, Material };
    void DrawShapes(DrawType drawType, const glm::mat4 & view, const glm::mat4 & projection);

    void RefreshShapeModels();
    void RefreshShapeModel(ShapeData & cube);

    std::unique_ptr<Shapes::Cube> m_cube;
    std::unique_ptr<Shapes::Sphere> m_sphere;

    Bullet m_world;
};
