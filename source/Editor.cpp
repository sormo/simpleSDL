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
        m_cubeScreenPosition = glm::vec2(Common::GetWindowWidth() / 2.0f, Common::GetWindowHeight() / 2.0f);
        m_gui.cubeDistance = 10.0f;

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

    return false;
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

    glm::vec2 delta = position - m_cursorPosition;
    m_cursorPosition = position;

    if (m_gui.cubeEditMode == UserInterface::CubeEditMode::Position)
        m_cubeScreenPosition += delta;
    else
        m_gui.cubeDistance += delta.y;

    ResetEditCube();

    return true;
}

void Editor::AddEditCube(const glm::vec3 & color)
{
    Material::Data material;
    material.ambient = material.diffuse = material.specular = color;
    material.shininess = 10.0f;

    m_gui.cubePosition = Common::GetPointWorldSpace(m_cubeScreenPosition, m_gui.cubeDistance, m_camera);

    glm::mat4 model(1.0f);
    model = glm::translate(model, m_gui.cubePosition);

    m_scene.AddCube(model, material);
}

void Editor::ResetEditCube()
{
    m_scene.PopCube();

    AddEditCube(EDIT_COLOR);
}
