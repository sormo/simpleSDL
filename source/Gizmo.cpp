#include "Gizmo.h"
#include "Common.h"

static const float AXIS_RADIUS = 0.05f;
static const float SPHERE_RADIUS = 0.1f;
static const float ARROW_RADIUS = 0.1f;

static const Material::Data RED_MATERIAL{ { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, 0.0f };
static const Material::Data GREEN_MATERIAL{ { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, 0.0f };
static const Material::Data BLUE_MATERIAL{ { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, 0.0f };
static const Material::Data WHITE_MATERIAL{ { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f }, 0.0f };

Gizmo::Gizmo(Scene& scene)
    : m_scene(scene), m_body(nullptr)
{
    CreateMoveGizmo();
    CreateScaleGizmo();
    CreateRotateGizmo();
}

void Gizmo::CreateMoveGizmo()
{
    std::vector<std::tuple<Shapes::Defintion::Sphere, Material::Data>> spheres;
    std::vector<std::tuple<Shapes::Defintion::Cylinder, Material::Data>> axes;
    std::vector<std::tuple<Shapes::Defintion::Cone, Material::Data>> arrows;

    spheres.push_back({ { glm::vec3(0.0f), glm::vec3(0.0f), SPHERE_RADIUS }, WHITE_MATERIAL });

    axes.push_back({ { { 0.0f, 0.5f, 0.0f }, glm::vec3(0.0f), AXIS_RADIUS, 1.0f }, RED_MATERIAL });
    axes.push_back({ { { 0.0f, 0.0f, 0.5f }, Common::Math::GetRotation(glm::radians(90.0f), { 1.0f, 0.0f, 0.0f }), AXIS_RADIUS, 1.0f }, GREEN_MATERIAL });
    axes.push_back({ { { 0.5f, 0.0f, 0.0f }, Common::Math::GetRotation(-glm::radians(90.0f), { 0.0f, 0.0f, 1.0f }), AXIS_RADIUS, 1.0f }, BLUE_MATERIAL });

    arrows.push_back({ { { 0.0f, 1.2f, 0.0f }, glm::vec3(0.0f), ARROW_RADIUS, 0.4f }, RED_MATERIAL });
    arrows.push_back({ { { 0.0f, 0.0f, 1.2f }, Common::Math::GetRotation(glm::radians(90.0f), { 1.0f, 0.0f, 0.0f }), ARROW_RADIUS, 0.4f }, GREEN_MATERIAL });
    arrows.push_back({ { { 1.2f, 0.0f, 0.0f }, Common::Math::GetRotation(-glm::radians(90.0f), { 0.0f, 0.0f, 1.0f }), ARROW_RADIUS, 0.4f }, BLUE_MATERIAL });

    //if (m_body)
    //    m_scene.RemoveBody(m_body);
    m_body = m_scene.AddCompound({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, false, {}, spheres, axes, arrows);
}

void Gizmo::CreateScaleGizmo()
{
    std::vector<std::tuple<Shapes::Defintion::Sphere, Material::Data>> spheres;
    std::vector<std::tuple<Shapes::Defintion::Cylinder, Material::Data>> axes;
    std::vector<std::tuple<Shapes::Defintion::Box, Material::Data>> arrows;

    spheres.push_back({ { glm::vec3(0.0f), glm::vec3(0.0f), SPHERE_RADIUS }, WHITE_MATERIAL });

    axes.push_back({ { { 0.0f, 0.5f, 0.0f }, glm::vec3(0.0f), AXIS_RADIUS, 1.0f }, RED_MATERIAL });
    axes.push_back({ { { 0.0f, 0.0f, 0.5f }, Common::Math::GetRotation(glm::radians(90.0f), { 1.0f, 0.0f, 0.0f }), AXIS_RADIUS, 1.0f }, GREEN_MATERIAL });
    axes.push_back({ { { 0.5f, 0.0f, 0.0f }, Common::Math::GetRotation(-glm::radians(90.0f), { 0.0f, 0.0f, 1.0f }), AXIS_RADIUS, 1.0f }, BLUE_MATERIAL });

    arrows.push_back({ { { 0.0f, 1.1f, 0.0f }, glm::vec3(0.0f), glm::vec3{ 0.2f } }, RED_MATERIAL });
    arrows.push_back({ { { 0.0f, 0.0f, 1.1f }, Common::Math::GetRotation(glm::radians(90.0f), { 1.0f, 0.0f, 0.0f }), glm::vec3{ 0.2f } }, GREEN_MATERIAL });
    arrows.push_back({ { { 1.1f, 0.0f, 0.0f }, Common::Math::GetRotation(-glm::radians(90.0f), { 0.0f, 0.0f, 1.0f }), glm::vec3{ 0.2f } }, BLUE_MATERIAL });

    //if (m_body)
    //    m_scene.RemoveBody(m_body);
    m_body = m_scene.AddCompound({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, false, arrows, spheres, axes, {});
}

void Gizmo::CreateRotateGizmo()
{
    std::vector<std::tuple<Shapes::Defintion::Sphere, Material::Data>> spheres;
    std::vector<std::tuple<Shapes::Defintion::Cylinder, Material::Data>> axes;

    spheres.push_back({ { glm::vec3(0.0f), glm::vec3(0.0f), SPHERE_RADIUS }, WHITE_MATERIAL });

    enum class RotationAxis { X, Y, Z };
    auto GenerateTorus = [this](float innerRadius, float outerRadius, uint32_t subdivisions, RotationAxis axis)
    {
        btVector3 forward, side;

        if (axis == RotationAxis::X)
        {
            forward = { 1.0f, 0.0f, 0.0f };
            side = { 0.0f, 0.0f, outerRadius };
        }
        else if (axis == RotationAxis::Z)
        {
            forward = { 0.0f, 0.0f, 1.0f };
            side = { outerRadius, 0.0f, 0.0f };
        }
        else if (axis == RotationAxis::Y)
        {
            forward = { 0.0f, 1.0f, 0.0f };
            side = { outerRadius, 0.0f, 0.0f };
        }

        float gap = (float)sqrt(2.0 * innerRadius * innerRadius - 2.0 * innerRadius * innerRadius * cos((2.0 * SIMD_PI) / (float)subdivisions));
        float radius = innerRadius;
        float height = 2.0f*(SIMD_PI / (float)subdivisions + 0.5f * gap);

        std::vector<Shapes::Defintion::Cylinder> definitions;

        for (uint32_t x = 0; x < subdivisions; x++)
        {
            btScalar angle = btScalar((x * 2.0 * SIMD_PI) / (float)subdivisions);
            btVector3 position = side.rotate(forward, angle);

            glm::vec3 rotation;
            btQuaternion(forward, angle).getEulerZYX(rotation.x, rotation.y, rotation.z);

            if (axis == RotationAxis::Y)
            {
                rotation += Common::Math::GetRotation(glm::radians(90.0f), {1.0f, 0.0f, 0.0f});
            }

            definitions.push_back({ { position.x(), position.y(), position.z() }, rotation, radius, height });
        }

        return definitions;
    };

    auto definitionsX = GenerateTorus(AXIS_RADIUS, 1.0f, 32, RotationAxis::X);
    auto definitionsY = GenerateTorus(AXIS_RADIUS, 1.0f, 32, RotationAxis::Y);
    auto definitionsZ = GenerateTorus(AXIS_RADIUS, 1.0f, 32, RotationAxis::Z);

    for (auto def : definitionsX)
        axes.push_back({ def, RED_MATERIAL });

    for (auto def : definitionsY)
        axes.push_back({ def, GREEN_MATERIAL });

    for (auto def : definitionsZ)
        axes.push_back({ def, BLUE_MATERIAL });

    //if (m_body)
    //    m_scene.RemoveBody(m_body);
    m_body = m_scene.AddCompound({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, false, {}, spheres, axes, {});
}
