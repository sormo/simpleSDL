#pragma once
#include "DebugDraw.h"
#include "Scene.h"

class Editor;

class EditorDebug
{
public:
    EditorDebug(Editor& editor);

    void Draw(const glm::mat4& view, const glm::mat4& projection);
    void Clear();

    void Ray(const glm::vec3& from, const glm::vec3& direction, const std::vector<Scene::RayCastResult>& raycast);

    void EditPlane(const Common::Math::Plane& plane, const glm::vec3 & center);
    void EditPlaneClear();

private:
    struct RayCast
    {
        glm::vec3 from;
        glm::vec3 direction;
        std::vector<glm::vec3> collisions;
    };

    std::vector<RayCast> m_raycasts;

    std::vector<glm::vec3> m_editPlane;

    Editor& m_editor;
    DebugDraw m_debugDraw;
};
