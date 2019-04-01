#include "Editor.h"
#include "glm/gtc/matrix_transform.hpp"

static const glm::vec3 EDIT_COLOR(0.0f, 1.0f, 0.0f);
static const glm::vec3 ADD_COLOR(1.0f, 1.0f, 1.0f);

Editor::Editor(Scene & scene, UserInterface & userInterface, Camera & camera)
    : m_scene(scene), m_gui(userInterface), m_camera(camera)
{
    m_gui.cubeEditClicked = [this]()
    {
        m_gui.cubePosition = Common::GetPointWorldSpace(glm::vec2(Common::GetWindowWidth() / 2.0f, Common::GetWindowHeight() / 2.0f), 10.0f, m_camera);
        m_gui.cubeScale = glm::vec3(1.0f, 1.0f, 1.0f);
        m_gui.cubeRotation = glm::vec3(0.0f, 0.0f, 0.0f);

        if (m_gui.cubeEdit)
            AddEditCube(EDIT_COLOR);
        else
            m_scene.PopCube();
    };

    m_gui.cubeAcceptClicked = [this]()
    {
        m_scene.PopCube();

        AddEditCube(ADD_COLOR);

        m_gui.cubeEditClicked();
    };
}

bool Editor::Press(const glm::vec2 & position, int64_t id)
{
    m_isPressed = true;
    m_cursorPosition = position;

    if (!m_gui.cubeEdit || m_gui.cubeEditMode == UserInterface::CubeEditMode::Camera)
        return false;

    if (m_gui.cubeEditMode == UserInterface::CubeEditMode::Translate)
    {
        TranslateCube(position);
        ResetEditCube();
    }

    return true;
}

bool Editor::Release(const glm::vec2 &  position, int64_t id)
{
    m_isPressed = false;

    return false;
}

bool Editor::Move(const glm::vec2 & position, int64_t id)
{
    if (!m_gui.cubeEdit || !m_isPressed || m_gui.cubeEditMode == UserInterface::CubeEditMode::Camera)
        return false;

    if (m_gui.cubeEditMode == UserInterface::CubeEditMode::Translate)
        TranslateCube(position);
    else if (m_gui.cubeEditMode == UserInterface::CubeEditMode::Rotate)
        RotateCube(position);
    else if (m_gui.cubeEditMode == UserInterface::CubeEditMode::Scale)
        ScaleCube(position);

    ResetEditCube();

    return true;
}

Common::Math::Plane Editor::GetEditPlane()
{
    glm::vec3 a, b;

    switch (m_gui.cubeEditAxis)
    {
    case UserInterface::CubeEditAxis::XY:
        a = glm::vec3(1.0f, 0.0f, 0.0f);
        b = glm::vec3(0.0f, 1.0f, 0.0f);
        break;
    case UserInterface::CubeEditAxis::XZ:
        a = glm::vec3(1.0f, 0.0f, 0.0f);
        b = glm::vec3(0.0f, 0.0f, 1.0f);
        break;
    case UserInterface::CubeEditAxis::YZ:
        a = glm::vec3(0.0f, 1.0f, 0.0f);
        b = glm::vec3(0.0f, 0.0f, 1.0f);
        break;
    }

    return Common::Math::Plane::CreateFromPoints({0.0f, 0.0f, 0.0f}, a, b);
}

void Editor::ScaleCube(const glm::vec2 & position)
{
    Common::Math::Plane plane = GetEditPlane();

    plane.Translate(m_gui.cubePosition);
    plane.Rotate(m_gui.cubeRotation);

    glm::vec3 p1;
    {
        Common::Math::Line ray = Common::GetRay(m_cursorPosition, m_camera);
        p1 = Common::Math::GetIntersection(plane, ray);
    }
    glm::vec3 p2;
    {
        Common::Math::Line ray = Common::GetRay(position, m_camera);
        p2 = Common::Math::GetIntersection(plane, ray);
    }

    glm::vec3 scaleVector(p1 - p2);

    m_gui.cubeScale += scaleVector;
    
    static const float MINIMUM_SCALE = 0.1f;
    m_gui.cubeScale.x = m_gui.cubeScale.x < MINIMUM_SCALE ? MINIMUM_SCALE : m_gui.cubeScale.x;
    m_gui.cubeScale.y = m_gui.cubeScale.y < MINIMUM_SCALE ? MINIMUM_SCALE : m_gui.cubeScale.y;
    m_gui.cubeScale.z = m_gui.cubeScale.z < MINIMUM_SCALE ? MINIMUM_SCALE : m_gui.cubeScale.z;

    m_cursorPosition = position;
}

void Editor::RotateCube(const glm::vec2 & position)
{
    glm::vec2 delta = position - m_cursorPosition;
    m_cursorPosition = position;

    m_gui.cubeRotation.y += glm::radians(delta.x);
    m_gui.cubeRotation.x += glm::radians(delta.y);
}

void Editor::TranslateCube(const glm::vec2 & position)
{
    // ***********************************************************************
    // // get point on line A and vector of line v
    // glm::vec3 A(m_camera.GetPosition());
    // glm::vec3 v = Common::GetPointWorldSpace(position, 10.0f, m_camera) - A;
    // // compute parameter in parametric equation of line according to constant axis
    // float t = (m_gui.cubePosition.z - A.z) / v.z;
    // glm::vec3 result;
    // result.x = A.x + v.x * t;
    // result.y = A.y + v.y * t;
    // result.z = m_gui.cubePosition.z;
    // ***********************************************************************

    Common::Math::Line ray = Common::GetRay(position, m_camera);
    Common::Math::Plane plane = GetEditPlane();

    plane.Translate(m_gui.cubePosition);

    m_gui.cubePosition = Common::Math::GetIntersection(plane, ray);
}

void Editor::AddEditCube(const glm::vec3 & color)
{
    Material::Data material;
    material.ambient = material.diffuse = material.specular = color;
    material.shininess = 10.0f;

    glm::mat4 model(1.0f);
 
    model = glm::translate(model, m_gui.cubePosition);

    model = glm::rotate(model, m_gui.cubeRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, m_gui.cubeRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, m_gui.cubeRotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

    model = glm::scale(model, m_gui.cubeScale);

    m_scene.AddCube(model, material);
}

void Editor::ResetEditCube()
{
    m_scene.PopCube();

    AddEditCube(EDIT_COLOR);
}
