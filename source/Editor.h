#pragma once
#include "Scene.h"
#include "UserInterface.h"
#include <SDL.h>
#include "MouseDispatcher.h"
#include "Camera.h"

class Editor : public MouseReceiver
{
public:
    Editor(Scene & scene, UserInterface & userInterface, Camera & camera);

    bool Press(const glm::vec2 & position, int64_t id) override;
    bool Release(const glm::vec2 &  position, int64_t id) override;
    bool Move(const glm::vec2 &  position, int64_t id) override;

private:
    void ResetEditCube();
    void AddEditCube(const glm::vec3 & color);

    void TranslateCube(const glm::vec2 & position);
    void RotateCube(const glm::vec2 & position);
    void ScaleCube(const glm::vec2 & position);

    // Line is defined by 'A' (point), 't' (vector)
    // Plane is defined by 'point' and is parallel to constant plane (m_gui.cubeEditAxis)
    glm::vec3 GetPointOfLineAndPlane(const glm::vec3 & A, glm::vec3 & v, const glm::vec3 & point);

    Scene & m_scene;
    UserInterface & m_gui;
    Camera & m_camera;

    bool m_isPressed = false;
    glm::vec2 m_cursorPosition;
};
