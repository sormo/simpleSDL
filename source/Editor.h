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

    Scene & m_scene;
    UserInterface & m_gui;
    Camera & m_camera;

    glm::vec2 m_cursorPosition;
    glm::vec2 m_cubeScreenPosition;
    bool m_isPressed = false;
};
