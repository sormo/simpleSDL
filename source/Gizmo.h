#pragma once
#include "Scene.h"

class Gizmo
{
public:
    Gizmo(Scene & scene);
private:

    void CreateMoveGizmo();
    void CreateScaleGizmo();
    void CreateRotateGizmo();

    Scene & m_scene;
    Scene::Body m_body;
};
