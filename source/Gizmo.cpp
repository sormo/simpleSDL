#include "Gizmo.h"
#include "Common.h"

static const float AXIS_RADIUS = 0.05f;
static const float SPHERE_RADIUS = 0.1f;
static const float ARROW_RADIUS = 0.1f;

static const Material::Data RED_MATERIAL{ { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, 0.0f };
static const Material::Data GREEN_MATERIAL{ { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, 0.0f };
static const Material::Data BLUE_MATERIAL{ { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, 0.0f };
static const Material::Data WHITE_MATERIAL{ { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f }, 0.0f };

static const glm::vec3 RED_ROTATION = glm::vec3(0.0f);
static const glm::vec3 GREEN_ROTATION = Common::Math::GetRotation(glm::radians(90.0f), { 1.0f, 0.0f, 0.0f });
static const glm::vec3 BLUE_ROTATION = Common::Math::GetRotation(-glm::radians(90.0f), { 0.0f, 0.0f, 1.0f });

Gizmo::Gizmo(Scene& scene)
    : m_scene(scene), m_body(nullptr)
{

}

void Gizmo::CreateMoveGizmo()
{
    m_body = m_scene.AddCompound({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, true, {}, {}, {}, {});
    m_centralSphere = m_scene.AddShape(m_body, Shapes::Defintion::Sphere{ glm::vec3(0.0f), glm::vec3(0.0f), SPHERE_RADIUS }, WHITE_MATERIAL);

    m_redShapes.insert(m_scene.AddShape(m_body, Shapes::Defintion::Cylinder{ { 0.0f, 0.5f, 0.0f }, RED_ROTATION, AXIS_RADIUS, 1.0f }, RED_MATERIAL));
    m_greenShapes.insert(m_scene.AddShape(m_body, Shapes::Defintion::Cylinder{ { 0.0f, 0.0f, 0.5f }, GREEN_ROTATION, AXIS_RADIUS, 1.0f }, GREEN_MATERIAL));
    m_blueShapes.insert(m_scene.AddShape(m_body, Shapes::Defintion::Cylinder{ { 0.5f, 0.0f, 0.0f }, BLUE_ROTATION, AXIS_RADIUS, 1.0f }, BLUE_MATERIAL));

    m_redShapes.insert(m_scene.AddShape(m_body, Shapes::Defintion::Cone{ { 0.0f, 1.2f, 0.0f }, RED_ROTATION, ARROW_RADIUS, 0.4f }, RED_MATERIAL));
    m_greenShapes.insert(m_scene.AddShape(m_body, Shapes::Defintion::Cone{ { 0.0f, 0.0f, 1.2f }, GREEN_ROTATION, ARROW_RADIUS, 0.4f }, GREEN_MATERIAL));
    m_blueShapes.insert(m_scene.AddShape(m_body, Shapes::Defintion::Cone{ { 1.2f, 0.0f, 0.0f }, BLUE_ROTATION, ARROW_RADIUS, 0.4f }, BLUE_MATERIAL));
}

void Gizmo::CreateScaleGizmo()
{
    m_body = m_scene.AddCompound({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, true, {}, {}, {}, {});
    m_centralSphere = m_scene.AddShape(m_body, Shapes::Defintion::Sphere{ glm::vec3(0.0f), glm::vec3(0.0f), SPHERE_RADIUS }, WHITE_MATERIAL);

    m_redShapes.insert(m_scene.AddShape(m_body, Shapes::Defintion::Cylinder{ { 0.0f, 0.5f, 0.0f }, RED_ROTATION, AXIS_RADIUS, 1.0f }, RED_MATERIAL));
    m_greenShapes.insert(m_scene.AddShape(m_body, Shapes::Defintion::Cylinder{ { 0.0f, 0.0f, 0.5f }, GREEN_ROTATION, AXIS_RADIUS, 1.0f }, GREEN_MATERIAL));
    m_blueShapes.insert(m_scene.AddShape(m_body, Shapes::Defintion::Cylinder{ { 0.5f, 0.0f, 0.0f }, BLUE_ROTATION, AXIS_RADIUS, 1.0f }, BLUE_MATERIAL));

    m_redShapes.insert(m_scene.AddShape(m_body, Shapes::Defintion::Box{ { 0.0f, 1.1f, 0.0f }, RED_ROTATION, glm::vec3{ 0.2f } }, RED_MATERIAL));
    m_greenShapes.insert(m_scene.AddShape(m_body, Shapes::Defintion::Box{ { 0.0f, 0.0f, 1.1f }, GREEN_ROTATION, glm::vec3{ 0.2f } }, GREEN_MATERIAL));
    m_blueShapes.insert(m_scene.AddShape(m_body, Shapes::Defintion::Box{ { 1.1f, 0.0f, 0.0f }, BLUE_ROTATION, glm::vec3{ 0.2f } }, BLUE_MATERIAL));
}

void Gizmo::CreateRotateGizmo()
{
    m_body = m_scene.AddCompound({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, true, {}, {}, {}, {});
    m_centralSphere = m_scene.AddShape(m_body, Shapes::Defintion::Sphere{ glm::vec3(0.0f), glm::vec3(0.0f), SPHERE_RADIUS }, WHITE_MATERIAL);

    enum class RotationAxis { X, Y, Z };
    auto GenerateTorus = [this](float innerRadius, float outerRadius, uint32_t subdivisions, RotationAxis axis)
    {
        btVector3 rotateAxis, side;

        if (axis == RotationAxis::X)
        {
            rotateAxis = { 1.0f, 0.0f, 0.0f };
            side = { 0.0f, 0.0f, outerRadius };
        }
        else if (axis == RotationAxis::Z)
        {
            rotateAxis = { 0.0f, 0.0f, 1.0f };
            side = { outerRadius, 0.0f, 0.0f };
        }
        else if (axis == RotationAxis::Y)
        {
            rotateAxis = { 0.0f, 1.0f, 0.0f };
            side = { outerRadius, 0.0f, 0.0f };
        }

        float gap = (float)sqrt(2.0 * innerRadius * innerRadius - 2.0 * innerRadius * innerRadius * cos((2.0 * SIMD_PI) / (float)subdivisions));
        float radius = innerRadius;
        float height = 2.0f*(SIMD_PI / (float)subdivisions + 0.5f * gap);

        std::vector<Shapes::Defintion::Cylinder> definitions;

        for (uint32_t x = 0; x < subdivisions; x++)
        {
            btScalar angle = btScalar((x * 2.0 * SIMD_PI) / (float)subdivisions);
            btVector3 position = side.rotate(rotateAxis, angle);

            glm::vec3 rotation;
            btQuaternion(rotateAxis, angle).getEulerZYX(rotation.x, rotation.y, rotation.z);

            if (axis == RotationAxis::Y)
            {
                rotation += Common::Math::GetRotation(glm::radians(90.0f), {1.0f, 0.0f, 0.0f});
            }

            definitions.push_back({ { position.x(), position.y(), position.z() }, rotation, radius, height });
        }

        return definitions;
    };

    auto definitionsX = GenerateTorus(AXIS_RADIUS, 1.0f, 32, RotationAxis::Y);
    auto definitionsY = GenerateTorus(AXIS_RADIUS, 1.0f, 32, RotationAxis::Z);
    auto definitionsZ = GenerateTorus(AXIS_RADIUS, 1.0f, 32, RotationAxis::X);

    for (auto def : definitionsX)
        m_redShapes.insert(m_scene.AddShape(m_body, def, RED_MATERIAL));

    for (auto def : definitionsY)
        m_greenShapes.insert(m_scene.AddShape(m_body, def, GREEN_MATERIAL));

    for (auto def : definitionsZ)
        m_blueShapes.insert(m_scene.AddShape(m_body, def, BLUE_MATERIAL));
}

Gizmo::Axis Gizmo::GetSelectedAxis()
{
    return m_selectedAxis;
}

void Gizmo::SetMode(Mode mode)
{
    if (mode == m_mode)
        return;

    ClearBody();

    switch (mode)
    {
    case Mode::Move:
        CreateMoveGizmo();
        break;
    case Mode::Scale:
        CreateScaleGizmo();
        break;
    case Mode::Rotate:
        CreateRotateGizmo();
        break;
    }

    m_mode = mode;
}

Gizmo::Mode Gizmo::GetMode()
{
    return m_mode;
}

void Gizmo::UpdateSelectedAxis(const std::vector<Scene::Shape>& shapes)
{
    for (auto shape : shapes)
    {
        if (auto it = m_redShapes.find(shape); it != std::end(m_redShapes))
        {
            m_selectedAxis = Axis::Y;
            return;
        }
        else if (auto it = m_greenShapes.find(shape); it != std::end(m_greenShapes))
        {
            m_selectedAxis = Axis::Z;
            return;
        }
        else if (auto it = m_blueShapes.find(shape); it != std::end(m_blueShapes))
        {
            m_selectedAxis = Axis::X;
            return;
        }
    }

    m_selectedAxis = Axis::None;
}

void Gizmo::UpdateBody(Scene::Body body)
{
    if (m_body)
    {
        m_body->SetPosition(body->GetPosition());

        if (m_mode == Mode::Scale || m_mode == Mode::Rotate)
            m_body->SetRotation(body->GetRotation());
    }
}

void Gizmo::ClearBody()
{
    if (m_body)
        m_scene.RemoveBody(m_body);
    m_body = nullptr;
    m_centralSphere = nullptr;
    m_redShapes.clear();
    m_greenShapes.clear();
    m_blueShapes.clear();
}

void Gizmo::FilterGizmoShapes(std::vector<Scene::Shape>& shapes)
{
    auto it = std::begin(shapes);

    while (it != std::end(shapes))
    {
        if (*it == m_centralSphere)
            it = shapes.erase(it);
        else if (m_redShapes.find(*it) != std::end(m_redShapes))
            it = shapes.erase(it);
        else if (m_greenShapes.find(*it) != std::end(m_greenShapes))
            it = shapes.erase(it);
        else if (m_blueShapes.find(*it) != std::end(m_blueShapes))
            it = shapes.erase(it);
        else
            it++;
    }
}
