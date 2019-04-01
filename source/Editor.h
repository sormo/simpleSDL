#pragma once
#include "Scene.h"
#include "UserInterface.h"
#include <SDL.h>
#include "MouseDispatcher.h"
#include "Camera.h"
#include "Common.h"

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

    Common::Math::Plane GetEditPlane();

    Scene & m_scene;
    UserInterface & m_gui;
    Camera & m_camera;

    bool m_isPressed = false;
    glm::vec2 m_cursorPosition;
};
