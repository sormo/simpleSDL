#include "Scene.h"
#include "glm/gtc/matrix_transform.hpp"

static const glm::vec3 WORLD_GRAVITY(0.0f, -10.0f, 0.0f);

struct SceneShapeHandle
{
    std::map<Shapes::Shape *, Scene::ShapeData>::iterator it;
};

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
}

Scene::~Scene()
{

}

Scene::Shape Scene::AddCube(const glm::vec3 & position, const glm::vec3 & rotation, const glm::vec3 & scale, const Material::Data & material, bool isStatic)
{
    btRigidBody * body = m_world.AddBox(position, rotation, scale, isStatic);

    ShapeData data;
    data.body = body;
    data.handle.reset(new SceneShapeHandle);
    data.material = material;
    data.model = glm::mat4(1.0f);
    data.scale = scale;

    auto it = m_shapes.insert({ m_cube.get(), std::move(data) });
    it->second.handle->it = it;

    RefreshShapeModel(it->second);

    return it->second.handle.get();
}

Scene::Shape Scene::AddSphere(const glm::vec3 & position, float radius, const Material::Data & material, bool isStatic)
{
    btRigidBody * body = m_world.AddSphere(position, radius, isStatic);

    ShapeData data;
    data.body = body;
    data.handle.reset(new SceneShapeHandle);
    data.material = material;
    data.model = glm::mat4(1.0f);
    data.scale = glm::vec3(radius);

    auto it = m_shapes.insert({ m_sphere.get(), std::move(data) });
    it->second.handle->it = it;

    RefreshShapeModel(it->second);

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
