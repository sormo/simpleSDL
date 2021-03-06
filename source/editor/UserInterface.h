#pragma once
#include "glm/glm.hpp"
#include <functional>
#include "imgui/imgui.h"
#include "model/ModelShader.h"
#include "utils/Camera.h"

struct UserInterface
{
    UserInterface(CameraRotate & camera);

    bool wireframe = false;
    bool bulletDebug = false;

    enum class ShapeEditType { None, Cube, Sphere, Cylinder, Cone } shapeEditType;
    enum class ShapeEditMode { Translate, Rotate, Scale } shapeEditMode;

    std::function<void()> shapeEditTypeClicked;
    std::function<void()> shapeEditModeClicked;
    std::function<void()> shapeAcceptClicked;
    std::function<void()> shapeCameraClicked;
    std::function<void()> shapeMaterialChanged;

    glm::vec3 shapePosition;
    glm::vec3 shapeRotation;
    glm::vec3 shapeScale;

    bool isStatic = true;

    Light::Data lightData;
    Material::Data materialData;
    glm::vec4 clearColor;
    glm::vec2 cameraPlanes;

    void Generate();

    CameraRotate& m_camera;
};
