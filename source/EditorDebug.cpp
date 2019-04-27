#include "EditorDebug.h"
#include "Editor.h"

EditorDebug::EditorDebug(Editor& editor)
    : m_editor(editor)
{

}

void EditorDebug::Draw(const glm::mat4& view, const glm::mat4& projection)
{
    // raycasts
    for (const auto& ray : m_raycasts)
    {
        static const float RAY_LENGTH = 5000.0f;
        m_debugDraw.DrawLine(ray.from, ray.direction * RAY_LENGTH, { 1.0f, 1.0f, 1.0f, 0.5f });
        for (const auto& collision : ray.collisions)
            m_debugDraw.DrawPoint(collision, { 1.0f, 0.0f, 0.0f, 1.0f });

        if (ray.collisions.size() > 1)
            m_debugDraw.DrawLine(ray.collisions.front(), ray.collisions.back(), { 1.0f, 0.0f, 0.0f, 0.5f });
    }

    // edit plane
    if (m_editPlane)
    {
        const Common::Math::Plane & plane = std::get<0>(*m_editPlane);
        const glm::vec3 & center = std::get<1>(*m_editPlane);

        m_debugDraw.DrawRectangle(center, { 2.0f, 2.0f }, plane.normal, {0.0f, 1.0f, 0.0f, 0.5f});
    }

    // rotation axis
    if (m_rotationAxis)
    {
        m_debugDraw.DrawLine(std::get<0>(*m_rotationAxis), std::get<1>(*m_rotationAxis), { 1.0f, 0.0f, 0.0f, 0.5f });
    }

    // current axis
    if (m_rotationAxis)
    {
        m_debugDraw.DrawLine(std::get<0>(*m_currentAxis), std::get<1>(*m_currentAxis), { 0.0f, 1.0f, 0.0f, 0.5f });
    }

    m_debugDraw.Draw(view, projection);
}

void EditorDebug::Clear()
{
    m_debugDraw.Clear();
}

void EditorDebug::Ray(const glm::vec3& from, const glm::vec3& direction, const std::vector<Scene::RayCastResult>& raycast)
{
    RayCast result;

    result.from = from;
    result.direction = glm::normalize(direction);

    for (const auto& res : raycast)
        result.collisions.push_back(std::get<1>(res));

    m_raycasts.push_back(result);
}

void EditorDebug::EditPlane(const Common::Math::Plane& plane, const glm::vec3& center)
{
    m_editPlane = { plane, center };
}

void EditorDebug::EditPlaneClear()
{
    m_editPlane = std::nullopt;
}

void EditorDebug::RotationAxis(const glm::vec3& axis, const glm::vec3& center)
{
    glm::vec3 v = glm::normalize(axis);

    m_rotationAxis = { center + v * 5.0f, center + v * -5.0f };
}

void EditorDebug::CurrentAxis(const glm::vec3& axis, const glm::vec3& center)
{
    glm::vec3 v = glm::normalize(axis);

    m_currentAxis = { center + v * 5.0f, center + v * -5.0f };
}
