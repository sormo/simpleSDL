#pragma once
#include "glm/glm.hpp"
#include <functional>
#include "imgui/imgui.h"

struct UserInterface
{
    bool wireframe = false;
    bool bulletDebug = false;

    bool cubeEdit = false;
    std::function<void()> cubeEditClicked;
    std::function<void()> cubeAcceptClicked;
    glm::vec3 cubePosition;
    glm::vec3 cubeRotation;
    glm::vec3 cubeScale;
    enum class CubeEditMode { Translate, Rotate, Scale, Camera } cubeEditMode;
    enum class CubeEditAxis { XY, XZ, YZ } cubeEditAxis;

    void Generate();
};
