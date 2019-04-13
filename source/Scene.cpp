#include "Scene.h"
#include "glm/gtc/matrix_transform.hpp"

static const glm::vec3 WORLD_GRAVITY(0.0f, -10.0f, 0.0f);

glm::vec3 Scene::ShapeObject::GetPosition()
{
    auto p = it->second.body->getWorldTransform().getOrigin();
    return { p.x(), p.y(), p.z() };
}

glm::vec3 Scene::ShapeObject::GetRotation()
{
    btQuaternion q = it->second.body->getWorldTransform().getRotation();
    glm::vec3 r;
    
    q.getEulerZYX(r.x, r.y, r.z);

    return r;
}

glm::vec3 Scene::ShapeObject::GetScale()
{
    return it->second.scale;
}

Shapes::Type Scene::ShapeObject::GetType()
{
    return it->first->type;
}

bool Scene::ShapeObject::IsStatic()
{
    return it->second.body->isStaticObject();
}

Scene::Scene(const Light::Config & light)
    : m_world(WORLD_GRAVITY)
{
    ModelShader::Config config;

    config.light = light;
    config.shading = ModelShader::ShadingModel::BlinnPhong;
    config.flags = (uint32_t)ModelShader::Config::Flags::UseRuntimeMaterial;

    m_shader = std::make_unique<ModelShader>(config);
    m_cube = std::make_unique<Shapes::Cube>(*m_shader.get());
    m_sphere = std::make_unique<Shapes::Sphere>(*m_shader.get());
    m_cylinder = std::make_unique<Shapes::Cylinder>(*m_shader.get());
    m_cone = std::make_unique<Shapes::Cone>(*m_shader.get());
}

Scene::~Scene()
{

}

Scene::Shape Scene::AddCube(const glm::vec3 & position, const glm::vec3 & rotation, const glm::vec3 & scale, const Material::Data & material, bool isStatic)
{
    btRigidBody * body = m_world.AddBox(position, rotation, scale / 2.0f, isStatic);
    return AddCommon(scale, body, material, m_cube.get());
}

Scene::Shape Scene::AddSphere(const glm::vec3 & position, const glm::vec3 & rotation, float radius, const Material::Data & material, bool isStatic)
{
    btRigidBody * body = m_world.AddSphere(position, rotation, radius, isStatic);
    return AddCommon(glm::vec3(radius), body, material, m_sphere.get());
}

Scene::Shape Scene::AddCylinder(const glm::vec3 & position, const glm::vec3 & rotation, float radius, float height, const Material::Data & material, bool isStatic)
{
    btRigidBody * body = m_world.AddCylinder(position, rotation, radius, height, isStatic);
    return AddCommon(glm::vec3(radius, height, radius), body, material, m_cylinder.get());
}

Scene::Shape Scene::AddCone(const glm::vec3 & position, const glm::vec3 & rotation, float radius, float height, const Material::Data & material, bool isStatic)
{
    btRigidBody * body = m_world.AddCone(position, rotation, radius, height, isStatic);
    return AddCommon(glm::vec3(radius, height, radius), body, material, m_cone.get());
}

Scene::Shape Scene::AddCommon(const glm::vec3 & scale, btRigidBody * body, const Material::Data & material, Shapes::Shape * shape)
{
    ShapeData data;
    data.body = body;
    data.handle.reset(new ShapeObject);
    data.material = material;
    data.model = glm::mat4(1.0f);
    data.scale = scale;

    auto it = m_shapes.insert({ shape, std::move(data) });
    it->second.handle->it = it;

    RefreshShapeModel(it->second);

    body->setUserPointer(it->second.handle.get());

    return it->second.handle.get();
}

void Scene::RemoveShape(Shape shape)
{
    m_world.RemoveBody(shape->it->second.body);
    m_shapes.erase(shape->it);
}

void Scene::RefreshShapeModels()
{
    for (auto & shape : m_shapes)
    {
        // TODO some optimization, no need to get transform for sleeping bodies
        RefreshShapeModel(shape.second);
    }
}

void Scene::RefreshShapeModel(ShapeData & cube)
{
    cube.body->getWorldTransform().getOpenGLMatrix(&cube.model[0][0]);
    cube.model = glm::scale(cube.model, cube.scale);
}

void Scene::Step()
{
    m_world.Step();

    RefreshShapeModels();
}

void Scene::DrawShapes(DrawType drawType, const glm::mat4 & view, const glm::mat4 & projection)
{
    if (m_shapes.empty())
        return;

    auto it = m_shapes.begin();
    Shapes::Shape * shape = it->first;
    shape->Bind();

    while (it != std::end(m_shapes))
    {
        if (it->first != shape)
        {
            shape = it->first;
            shape->Bind();
        }

        while (it != std::end(m_shapes) && it->first == shape)
        {
            if (drawType == DrawType::Shadow)
            {
                m_shader->BindTransformShadow(it->second.model);
            }
            else
            {
                m_shader->BindMaterial(it->second.material);
                m_shader->BindTransform(it->second.model, view, projection);
            }

            shape->Draw();

            it++;
        }
    }

}

void Scene::Draw(const glm::mat4 & view, const glm::mat4 & projection, const glm::vec3 & cameraPosition, const Light::Data & data)
{
    m_shader->BeginRender();

    while (m_shader->BeginRenderShadow(data))
    {
        DrawShapes(DrawType::Shadow, view, projection);

        m_shader->EndRenderShadow();
    }

    m_shader->BindCamera(cameraPosition);
    m_shader->BindLight(data);

    DrawShapes(DrawType::Material, view, projection);

    m_shader->EndRender();
}

void Scene::DrawDebug(const glm::mat4 & view, const glm::mat4 & projection)
{
    m_world.DebugDraw(view, projection);
}

std::vector<Scene::Shape> Scene::RayCast(const glm::vec3 & position, const glm::vec3 & direction)
{
    std::vector<const btCollisionObject*> castResult = m_world.RayCast(position, direction);

    std::vector<Shape> result;
    for (const btCollisionObject* cobj : castResult)
    {
        btCollisionObject * obj = const_cast<btCollisionObject*>(cobj);
        result.push_back((Shape)obj->getUserPointer());
    }

    return result;
}
