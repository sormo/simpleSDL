#pragma once
#include "glm/glm.hpp"
#include <functional>
#include "imgui/imgui.h"

struct UserInterface
{
    bool wireframe = false;
    bool bulletDebug = false;

    enum class ShapeEditType { None, Cube, Circle, Cylinder, Cone } shapeEditType;
    enum class ShapeEditMode { Translate, Rotate, Scale, Camera } shapeEditMode;
    enum class ShapeEditAxis { XY, XZ, YZ } shapeEditAxis;

    std::function<void()> shapeEditClicked;
    std::function<void()> shapeAcceptClicked;

    glm::vec3 shapePosition;
    glm::vec3 shapeRotation;
    glm::vec3 shapeScale;

    void Generate();
};
