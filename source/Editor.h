#pragma once
#include "Scene.h"
#include "UserInterface.h"
#include <SDL.h>
#include "EventDispatchers.h"
#include "Camera.h"
#include "Common.h"
#include "Gizmo.h"
#include "EditorDebug.h"

class Editor : public MouseReceiver
{
    friend class EditorDebug;
public:
    Editor(Scene & scene, UserInterface & userInterface, Camera & camera);

    bool Press(const glm::vec2 & position, int64_t id) override;
    bool Release(const glm::vec2 &  position, int64_t id) override;
    bool Move(const glm::vec2 &  position, int64_t id) override;

    void Draw(const glm::mat4& view, const glm::mat4& projection);

private:
    void ResetEditShape();
    void AddEditShape(const glm::vec3 & color);
    void SetEditShape(Scene::Shape shape);
    Scene::Body AddBodyToScene(const glm::vec3& color);


    void TranslateShape(const glm::vec2 & position);
    void RotateShape(const glm::vec2 & position);
    void ScaleShape(const glm::vec2 & position);

    void Clicked(const glm::vec2 & position);

    Common::Math::Plane GetEditPlane();
    Common::Math::Plane GetRotatePlane();
    glm::vec3 RestrictToLine(const glm::vec3& planeIntersection);
    glm::vec3 GetEditLineUnit();

    Scene & m_scene;
    UserInterface & m_gui;
    Camera & m_camera;

    EditorDebug m_debug;

    Scene::Shape m_editShape = nullptr;

    glm::vec2 m_cursorPosition;
    glm::vec2 m_pressPosition;

    void UpdateGizmo();

    Gizmo m_gizmo;
    glm::vec3 m_gizmoOffset;
    void ComputeGizmoOffset(const glm::vec2& position);
};
