#pragma once
#include <memory>
#include <map>
#include <set>
#include "Bullet.h"
#include "Shapes.h"
#include "model/ModelShader.h"

class Scene
{
    struct ShapeData;
    struct BodyData;
public:
    Scene(const Light::Config & light);
    // desctructor must be implemented  where SceneShapeHandle is defined
    ~Scene();

    static const uint32_t ShapeFlagNoDraw = 0x0001;

    struct BodyHandle;
    struct ShapeHandle
    {
        friend class Scene;

        glm::vec3 GetPosition();
        glm::vec3 GetRotation();
        glm::vec3 GetScale();
        Shapes::Type GetType();
        BodyHandle * GetBody();
        uint32_t & GetFlags();
        const glm::mat4& GetTransform();
        Material::Data& GetMaterial();

    private:
        std::multimap<Shapes::Shape*, ShapeData>::iterator it;
    };
    using Shape = ShapeHandle*;

    struct BodyHandle
    {
        friend class Scene;

        glm::vec3 GetPosition();
        void SetPosition(const glm::vec3& position);

        glm::vec3 GetRotation();
        void SetRotation(const glm::vec3& rotation);

        bool IsStatic();
        bool IsCompound();
        const std::vector<Shape> & GetShapes();
    private:
        std::set<BodyData>::iterator it;
    };
    using Body = BodyHandle*;

    Body AddCube(const Shapes::Defintion::Box & definition, const Material::Data & material, bool isStatic);
    Body AddSphere(const Shapes::Defintion::Sphere & definition, const Material::Data & material, bool isStatic);
    Body AddCylinder(const Shapes::Defintion::Cylinder & definition, const Material::Data & material, bool isStatic);
    Body AddCone(const Shapes::Defintion::Cone & definition, const Material::Data & material, bool isStatic);
    Body AddCompound(const glm::vec3 & position, const glm::vec3 & rotation, bool isStatic,
        const std::vector<std::tuple<Shapes::Defintion::Box, Material::Data>> & box,
        const std::vector<std::tuple<Shapes::Defintion::Sphere, Material::Data>> & sphere,
        const std::vector<std::tuple<Shapes::Defintion::Cylinder, Material::Data>> & cylinder,
        const std::vector<std::tuple<Shapes::Defintion::Cone, Material::Data>> & cone);
    template<class T>
    Shape AddShape(Body compound, const T & definition, const Material::Data & material, uint32_t flags = 0);
    void RemoveBody(Body body);

    void Step();

    void Draw(const glm::mat4 & view, const glm::mat4 & projection, const glm::vec3 & cameraPosition, const Light::Data & data);
    void DrawDebug(const glm::mat4 & view, const glm::mat4 & projection);

    using RayCastResult = std::tuple<Shape, glm::vec3>;
    std::vector<RayCastResult> RayCast(const glm::vec3 & position, const glm::vec3 & direction);

    void Clear();

private:
    std::unique_ptr<ModelShader> m_shader;

    struct ShapeData
    {
        glm::mat4 model;
        glm::mat4 localTransform;
        glm::vec3 scale;
        Material::Data material;
        BodyHandle * body;
        btCollisionShape * shape;
        std::unique_ptr<ShapeHandle> handle;
        uint32_t flags = 0;
    };
    std::multimap<Shapes::Shape*, ShapeData> m_shapes;

    struct BodyData
    {
        btRigidBody * body;
        std::unique_ptr<BodyHandle> handle;
        std::vector<ShapeHandle*> shapes;

        bool operator<(const BodyData & o) const;
    };
    std::set<BodyData> m_bodies;

    enum class DrawType{ Shadow, Material };
    void DrawShapes(DrawType drawType, const glm::mat4 & view, const glm::mat4 & projection);

    void RefreshShapeModels();
    void RefreshShapeModel(ShapeData & cube);

    Scene::Shape AddShape(btCollisionShape * shape, const glm::mat4 & local, const glm::vec3 & scale, BodyHandle * body, const Material::Data & material, Shapes::Shape * drawShape, uint32_t flags);
    Scene::Body AddBody(btRigidBody * body);

    std::unique_ptr<Shapes::Cube> m_cube;
    std::unique_ptr<Shapes::Sphere> m_sphere;
    std::unique_ptr<Shapes::Cylinder> m_cylinder;
    std::unique_ptr<Shapes::Cone> m_cone;

    Bullet m_world;
};
