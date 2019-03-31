#include "Editor.h"
#include "Common.h"
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

glm::vec3 Editor::GetPointOfLineAndPlane(const glm::vec3 & A, glm::vec3 & v, const glm::vec3 & point)
{
    glm::vec3 t = (point - A) / v;
    glm::vec3 result = point;

    switch (m_gui.cubeEditAxis)
    {
    case UserInterface::CubeEditAxis::XY:
        result.x = A.x + v.x * t.z;
        result.y = A.y + v.y * t.z;
        break;
    case UserInterface::CubeEditAxis::XZ:
        result.x = A.x + v.x * t.y;
        result.z = A.z + v.z * t.y;
        break;
    case UserInterface::CubeEditAxis::YZ:
        result.z = A.z + v.z * t.x;
        result.y = A.y + v.y * t.x;
        break;
    }

    return result;
}

void Editor::ScaleCube(const glm::vec2 & position)
{
    static const float SCALE_FACTOR = 0.1f;

    glm::vec3 p1;
    {
        glm::vec3 A(m_camera.GetPosition());
        glm::vec3 v = Common::GetPointWorldSpace(m_cursorPosition, 10.0f, m_camera) - A;

        p1 = GetPointOfLineAndPlane(A, v, { 0.0f, 0.0f, 0.0f });
    }
    glm::vec3 p2;
    {
        glm::vec3 A(m_camera.GetPosition());
        glm::vec3 v = Common::GetPointWorldSpace(position, 10.0f, m_camera) - A;

        p2 = GetPointOfLineAndPlane(A, v, { 0.0f, 0.0f, 0.0f });
    }

    glm::vec3 scaleVector(p2 - p1);
    glm::mat4 model(1.0f);
    model = glm::rotate(model, m_gui.cubeRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, m_gui.cubeRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, m_gui.cubeRotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
    scaleVector = model * glm::vec4(scaleVector, 0.0f);

    m_gui.cubeScale += scaleVector;
    //m_gui.cubeScale += p2 - p1;

    //glm::vec2 delta = position - m_cursorPosition;
    m_cursorPosition = position;

    //m_gui.cubeScale.x += delta.x * SCALE_FACTOR;
    //m_gui.cubeScale.y += delta.y * SCALE_FACTOR;


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

    glm::vec3 A(m_camera.GetPosition());
    glm::vec3 v = Common::GetPointWorldSpace(position, 10.0f, m_camera) - A;

    m_gui.cubePosition = GetPointOfLineAndPlane(A, v, m_gui.cubePosition);

    //glm::vec3 t = (m_gui.cubePosition - A) / v;

    //switch (m_gui.cubeEditAxis)
    //{
    //case UserInterface::CubeEditAxis::XY:
    //    m_gui.cubePosition.x = A.x + v.x * t.z;
    //    m_gui.cubePosition.y = A.y + v.y * t.z;
    //    break;
    //case UserInterface::CubeEditAxis::XZ:
    //    m_gui.cubePosition.x = A.x + v.x * t.y;
    //    m_gui.cubePosition.z = A.z + v.z * t.y;
    //    break;
    //case UserInterface::CubeEditAxis::YZ:
    //    m_gui.cubePosition.z = A.z + v.z * t.x;
    //    m_gui.cubePosition.y = A.y + v.y * t.x;
    //    break;
    //}
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
