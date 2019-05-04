#include "Scene.h"
#include "glm/gtc/matrix_transform.hpp"

static const glm::vec3 WORLD_GRAVITY(0.0f, -10.0f, 0.0f);

glm::vec3 Scene::BodyHandle::GetPosition()
{
    auto p = it->body->getWorldTransform().getOrigin();
    return { p.x(), p.y(), p.z() };
}

void Scene::BodyHandle::SetPosition(const glm::vec3& position)
{
    btTransform t(it->body->getWorldTransform());
    t.setOrigin({ position.x, position.y, position.z });

    it->body->setWorldTransform(t);
}

glm::vec3 Scene::BodyHandle::GetRotation()
{
    btQuaternion q = it->body->getWorldTransform().getRotation();
    glm::vec3 r;

    q.getEulerZYX(r.x, r.y, r.z);

    return r;
}

void Scene::BodyHandle::SetRotation(const glm::vec3& rotation)
{
    btTransform t(it->body->getWorldTransform());
    btQuaternion q;
    q.setEulerZYX(rotation.x, rotation.y, rotation.z);
    t.setRotation(q);

    it->body->setWorldTransform(t);
}

bool Scene::BodyHandle::IsStatic()
{
    return it->body->isStaticObject();
}

bool Scene::BodyHandle::IsCompound()
{
    return it->body->getCollisionShape()->isCompound();
}

const std::vector<Scene::Shape> & Scene::BodyHandle::GetShapes()
{
    return it->shapes;
}

glm::vec3 Scene::ShapeHandle::GetPosition()
{
    // TODO transform from compound shape
    return glm::vec3(0.0f);
}

glm::vec3 Scene::ShapeHandle::GetRotation()
{
    // TODO transform from compound shape
    return glm::vec3(0.0f);
}

glm::vec3 Scene::ShapeHandle::GetScale()
{
    return it->second.scale;
}

Shapes::Type Scene::ShapeHandle::GetType()
{
    return it->first->type;
}

Scene::BodyHandle * Scene::ShapeHandle::GetBody()
{
    return it->second.body;
}

uint32_t& Scene::ShapeHandle::GetFlags()
{
    return it->second.flags;
}

const glm::mat4& Scene::ShapeHandle::GetTransform()
{
    return it->second.model;
}

Scene::Scene(const Light::Config & light)
    : m_world(WORLD_GRAVITY)
{
    ModelShader::Config config;

    config.light = light;
    config.shading = ModelShader::ShadingModel::BlinnPhong;
    config.flags = (uint32_t)ModelShader::Config::Flags::UseRuntimeMaterial;

    m_shader = std::make_unique<ModelShader>(config);
    m_cube = std::make_unique<Shapes::Cube>(m_shader.get());
    m_sphere = std::make_unique<Shapes::Sphere>(m_shader.get());
    m_cylinder = std::make_unique<Shapes::Cylinder>(m_shader.get());
    m_cone = std::make_unique<Shapes::Cone>(m_shader.get());
}

Scene::~Scene()
{

}

Scene::Body Scene::AddCube(const Shapes::Defintion::Box & definition, const Material::Data & material, bool isStatic)
{
    Body body = AddBody(m_world.AddBox(definition, isStatic));
    AddShape(body->it->body->getCollisionShape(), glm::mat4(1.0f), definition.extents, body, material, m_cube.get(), 0);

    return body;
}

Scene::Body Scene::AddSphere(const Shapes::Defintion::Sphere & definition, const Material::Data & material, bool isStatic)
{
    Body body = AddBody(m_world.AddSphere(definition, isStatic));
    AddShape(body->it->body->getCollisionShape(), glm::mat4(1.0f), glm::vec3(definition.radius), body, material, m_sphere.get(), 0);

    return body;
}

Scene::Body Scene::AddCylinder(const Shapes::Defintion::Cylinder & definition, const Material::Data & material, bool isStatic)
{
    Body body = AddBody(m_world.AddCylinder(definition, isStatic));
    AddShape(body->it->body->getCollisionShape(), glm::mat4(1.0f), glm::vec3(definition.radius, definition.height, definition.radius), body, material, m_cylinder.get(), 0);

    return body;
}

Scene::Body Scene::AddCone(const Shapes::Defintion::Cone & definition, const Material::Data & material, bool isStatic)
{
    Body body = AddBody(m_world.AddCone(definition, isStatic));
    AddShape(body->it->body->getCollisionShape(), glm::mat4(1.0f), glm::vec3(definition.radius, definition.height, definition.radius), body, material, m_cone.get(), 0);

    return body;
}

template<class T>
glm::mat4 GetTransform(const T & transform)
{
    btQuaternion quatRotation;
    quatRotation.setEulerZYX(transform.rotation.x, transform.rotation.y, transform.rotation.z);
    btTransform tmp(quatRotation, { transform.position.x, transform.position.y, transform.position.z });

    glm::mat4 result;
    tmp.getOpenGLMatrix(&result[0][0]);

    return result;
}

Scene::Body Scene::AddCompound(const glm::vec3 & position, const glm::vec3 & rotation, bool isStatic,
    const std::vector<std::tuple<Shapes::Defintion::Box, Material::Data>> & box,
    const std::vector<std::tuple<Shapes::Defintion::Sphere, Material::Data>> & sphere,
    const std::vector<std::tuple<Shapes::Defintion::Cylinder, Material::Data>> & cylinder,
    const std::vector<std::tuple<Shapes::Defintion::Cone, Material::Data>> & cone)
{
    btRigidBody * worldBody = m_world.AddCompound(position, rotation, isStatic);
    Body body = AddBody(worldBody);

    for (const auto&[definition, material] : box)
        AddShape(m_world.AddShape(worldBody, definition), GetTransform(definition), definition.extents, body, material, m_cube.get(), 0);

    for (const auto&[definition, material] : sphere)
        AddShape(m_world.AddShape(worldBody, definition), GetTransform(definition), glm::vec3(definition.radius), body, material, m_sphere.get(), 0);

    for (const auto&[definition, material] : cylinder)
        AddShape(m_world.AddShape(worldBody, definition), GetTransform(definition), glm::vec3(definition.radius, definition.height, definition.radius), body, material, m_cylinder.get(), 0);

    for (const auto&[definition, material] : cone)
        AddShape(m_world.AddShape(worldBody, definition), GetTransform(definition), glm::vec3(definition.radius, definition.height, definition.radius), body, material, m_cone.get(), 0);

    return body;
}

Scene::Body Scene::AddBody(btRigidBody * body)
{
    BodyHandle * newBody = new BodyHandle;

    BodyData data;
    data.body = body;
    data.handle.reset(newBody);

    auto it = m_bodies.insert(std::move(data));
    it.first->handle->it = it.first;

    body->setUserPointer(newBody);

    return newBody;
}

Scene::Shape Scene::AddShape(btCollisionShape * shape, const glm::mat4 & localTransform, const glm::vec3 & scale, BodyHandle * body, const Material::Data & material, Shapes::Shape * drawShape, uint32_t flags)
{
    ShapeHandle * newShape = new ShapeHandle;

    ShapeData data;
    data.body = body;
    data.handle.reset(newShape);
    data.material = material;
    data.model = glm::mat4(1.0f);
    data.scale = scale;
    data.shape = shape;
    data.localTransform = localTransform;
    data.flags = flags;

    std::multimap<Shapes::Shape*, ShapeData>::iterator it = m_shapes.insert({ drawShape, std::move(data) });
    newShape->it = it;

    shape->setUserPointer(newShape);

    RefreshShapeModel(it->second);

    // TODO why need to const cast here ?
    std::vector<ShapeHandle*> & shapes = const_cast<std::vector<ShapeHandle*>&>(body->it->shapes);
    shapes.push_back(newShape);

    return newShape;
}

template<class T>
Scene::Shape Scene::AddShape(Body compound, const T& definition, const Material::Data& material, uint32_t flags)
{
    printf("Unsupported definition type fpr Scene AddShape");
    throw std::runtime_error("Unsupported definition type fpr Scene AddShape");
}

template<>
Scene::Shape Scene::AddShape(Body compound, const Shapes::Defintion::Box & definition, const Material::Data & material, uint32_t flags)
{
    return AddShape(m_world.AddShape(compound->it->body, definition), GetTransform(definition), 
        definition.extents, compound, material, m_cube.get(), flags);
}

template<>
Scene::Shape Scene::AddShape(Body compound, const Shapes::Defintion::Sphere & definition, const Material::Data & material, uint32_t flags)
{
    return AddShape(m_world.AddShape(compound->it->body, definition), GetTransform(definition), 
        glm::vec3(definition.radius), compound, material, m_sphere.get(), flags);
}

template<>
Scene::Shape Scene::AddShape(Body compound, const Shapes::Defintion::Cylinder & definition, const Material::Data & material, uint32_t flags)
{
    return AddShape(m_world.AddShape(compound->it->body, definition), GetTransform(definition),
        glm::vec3(definition.radius, definition.height, definition.radius), compound, material, m_cylinder.get(), flags);
}

template<>
Scene::Shape Scene::AddShape(Body compound, const Shapes::Defintion::Cone & definition, const Material::Data & material, uint32_t flags)
{
    return AddShape(m_world.AddShape(compound->it->body, definition), GetTransform(definition),
        glm::vec3(definition.radius, definition.height, definition.radius), compound, material, m_cone.get(), flags);
}

void Scene::RemoveBody(Body body)
{
    // remove all shapes
    for (ShapeHandle * shape : body->it->shapes)
        m_shapes.erase(shape->it);
    // remove body
    m_world.RemoveBody(body->it->body);
    m_bodies.erase(body->it);
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
    cube.body->it->body->getWorldTransform().getOpenGLMatrix(&cube.model[0][0]);

    cube.model = cube.model * cube.localTransform;

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
            if (it->second.flags & ShapeFlagNoDraw)
            {
                it++;
                continue;
            }

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

std::vector<std::tuple<Scene::Shape, glm::vec3>> Scene::RayCast(const glm::vec3 & position, const glm::vec3 & direction)
{
    auto castResult = m_world.RayCast(position, direction);

    std::vector<std::tuple<Scene::Shape, glm::vec3>> result;
    for (auto[_, shape, position] : castResult)
    {
        result.push_back({ (Shape)shape->getUserPointer(), position });
    }

    return result;
}

bool Scene::BodyData::operator<(const BodyData & o) const
{
    return body < o.body;
}
