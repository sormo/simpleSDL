#pragma once
#include "glm/glm.hpp"
#include <functional>
#include "imgui/imgui.h"

struct UserInterface
{
    ImVec2 windowSize;

    bool cubeEdit = false;
    std::function<void()> cubeEditClicked;
    std::function<void()> cubeAcceptClicked;
    float cubeDistance = 10.0f;
    glm::vec3 cubePosition;
    enum class CubeEditMode { Position, Distance, Camera } cubeEditMode;

    void Generate();
    UserInterface();
};
