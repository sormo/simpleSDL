#pragma once
#include "Scene.h"

class Gizmo
{
public:
    Gizmo(Scene & scene);

    enum class Axis
    {
        X,
        Y,
        Z,
        None
    };
    Axis GetSelectedAxis();

    enum class Mode
    {
        Move,
        Scale,
        Rotate,
        None
    };
    void SetMode(Mode mode);
    Mode GetMode();

    void UpdateSelectedAxis(const std::vector<Scene::RayCastResult> & shapes, const glm::vec3 & cameraPosition);
    void UpdateBody(Scene::Body body);

    void FilterGizmoShapes(std::vector<Scene::RayCastResult>& shapes);

private:

    void CreateMoveGizmo();
    void CreateScaleGizmo();
    void CreateRotateGizmo();

    Axis m_selectedAxis = Axis::None;
    Mode m_mode = Mode::None;

    Scene & m_scene;

    void ClearBody();

    Scene::Body m_body = nullptr;

    Scene::Shape m_centralSphere = nullptr;
    std::set<Scene::Shape> m_redShapes;
    std::set<Scene::Shape> m_greenShapes;
    std::set<Scene::Shape> m_blueShapes;
};
