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

    // editplane
    if (!m_editPlane.empty())
    {
        m_debugDraw.DrawTriangle(m_editPlane[0], m_editPlane[1], m_editPlane[2], { 0.0f, 1.0f, 0.0f, 0.5f });
        m_debugDraw.DrawTriangle(m_editPlane[3], m_editPlane[4], m_editPlane[5], { 0.0f, 1.0f, 0.0f, 0.5f });
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
    static const float HALF_EXTENT = 50.0f;

    m_editPlane.clear();

    auto SolveForZ = [](float x, float y, const Common::Math::Plane & plane)
    {
        return (plane.value - plane.normal.x * x - plane.normal.y * y) / plane.normal.z;
    };

    glm::vec3 rectangle[4] =
    {
        { -HALF_EXTENT,-HALF_EXTENT, SolveForZ(-HALF_EXTENT,-HALF_EXTENT, plane) },
        {  HALF_EXTENT, HALF_EXTENT, SolveForZ( HALF_EXTENT, HALF_EXTENT, plane) },
        {  HALF_EXTENT,-HALF_EXTENT, SolveForZ( HALF_EXTENT,-HALF_EXTENT, plane) },
        { -HALF_EXTENT, HALF_EXTENT, SolveForZ(-HALF_EXTENT, HALF_EXTENT, plane) }
    };

    glm::vec3 centerDisplacement(0.0f, 0.0f, SolveForZ(0.0f, 0.0f, plane));

    m_editPlane.push_back(rectangle[0] + center + centerDisplacement);
    m_editPlane.push_back(rectangle[1] + center + centerDisplacement);
    m_editPlane.push_back(rectangle[2] + center + centerDisplacement);

    m_editPlane.push_back(rectangle[0] + center + centerDisplacement);
    m_editPlane.push_back(rectangle[1] + center + centerDisplacement);
    m_editPlane.push_back(rectangle[3] + center + centerDisplacement);
}

void EditorDebug::EditPlaneClear()
{
    m_editPlane.clear();
}
