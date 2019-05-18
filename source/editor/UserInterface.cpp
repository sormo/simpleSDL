#include "UserInterface.h"
#include "Common.h"
#include <SDL.h>
#include "utils/Camera.h"

UserInterface::UserInterface(CameraRotate& camera)
    : shapeScale(1.0f, 1.0f, 1.0f), cameraPlanes(0.1f, 1000.0f), m_camera(camera)
{
    lightData.lightDirectional.ambient = { 0.1f, 0.1f, 0.1f };
    lightData.lightDirectional.diffuse = { 0.5f, 0.5f, 0.5f };
    lightData.lightDirectional.specular = { 0.3f, 0.3f, 0.3f };
    lightData.lightDirectional.shadowParams = { 0.001f, 0.001f };

    materialData.ambient = materialData.diffuse = materialData.specular = glm::vec3(1.0, 1.0, 1.0);
    materialData.shininess = 15.0f;
    materialData.shininessStrength = 1.0f;

    clearColor = glm::vec4(0.0f);
}

bool ToggleButton(const char * name, bool * toggled)
{
    bool result;

    if (*toggled)
    {
        ImGui::PushID("toggle button");
        ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(3 / 7.0f, 1.0f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(3 / 7.0f, 1.0f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(3 / 7.0f, 1.0f, 1.0f));
        result = ImGui::Button(name);
        ImGui::PopStyleColor(3);
        ImGui::PopID();
    }
    else
    {
        result = ImGui::Button(name);
    }

    if (result)
        *toggled = !(*toggled);

    return result;
}

void UserInterface::Generate()
{
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar;

    ImGui::SetNextWindowPos(ImVec2(5, 5), ImGuiCond_Always);
    ImGui::SetNextWindowSize({0.0f, 0.0f}, ImGuiCond_Always);

    ImGui::Begin("test", nullptr, windowFlags);

    ImGui::Text("Frame: %06.2fms", Common::Frame::GetFPS());
    
    int32_t mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    ImGui::Text("Screen: %03d:%03d", mouseX, mouseY);

    ImGui::Checkbox("Wireframe", &wireframe);
    ImGui::Checkbox("Bullet debug", &bulletDebug);

    ///////////////////////////////////////////////////////////////////////////

    if (ImGui::CollapsingHeader("Camera"))
    {
        bool modified = false;

        modified |= ImGui::SliderFloat("Near plane", &cameraPlanes.x, 0.1f, 100.0f, "%.4f", 1.0f);
        modified |= ImGui::SliderFloat("Far plane", &cameraPlanes.y, 1.0f, 1000.0f, "%.4f", 1.0f);

        if (modified)
            m_camera.SetPlanes(cameraPlanes);
    }

    ///////////////////////////////////////////////////////////////////////////

    if (ImGui::ColorEdit3("Clear", &clearColor[0]))
    {
        glClearColor(clearColor[0], clearColor[1], clearColor[2], 1.0f);
    }

    if (ImGui::CollapsingHeader("Light"))
    {
        ImGui::ColorEdit3("Ambient", &lightData.lightDirectional.ambient[0]);
        ImGui::ColorEdit3("Diffuse", &lightData.lightDirectional.diffuse[0]);
        ImGui::ColorEdit3("Specular", &lightData.lightDirectional.specular[0]);

        ImGui::SliderFloat("Shadow 1", &lightData.lightDirectional.shadowParams.x, 0.00001f, 0.01f, "%.5f", 1.0f);
        ImGui::SliderFloat("Shadow 2", &lightData.lightDirectional.shadowParams.y, 0.00001f, 0.01f, "%.5f", 1.0f);
    }

    ///////////////////////////////////////////////////////////////////////////

    bool cubeEdit = false, circleEdit = false, cylinderEdit = false, coneEdit = false;
    switch (shapeEditType)
    {
    case ShapeEditType::Cube:
        cubeEdit = true; break;
    case ShapeEditType::Sphere:
        circleEdit = true; break;
    case ShapeEditType::Cylinder:
        cylinderEdit = true; break;
    case ShapeEditType::Cone:
        coneEdit = true; break;
    default:
        break;
    }

    if (ToggleButton("Cube", &cubeEdit))
    {
        shapeEditType = cubeEdit ? ShapeEditType::Cube : ShapeEditType::None;
        shapeEditTypeClicked();
    }
    ImGui::SameLine();
    if (ToggleButton("Circle", &circleEdit))
    {
        shapeEditType = circleEdit ? ShapeEditType::Sphere : ShapeEditType::None;
        shapeEditTypeClicked();
    }
    if (ToggleButton("Cylinder", &cylinderEdit))
    {
        shapeEditType = cylinderEdit ? ShapeEditType::Cylinder : ShapeEditType::None;
        shapeEditTypeClicked();
    }
    ImGui::SameLine();
    if (ToggleButton("Cone", &coneEdit))
    {
        shapeEditType = coneEdit ? ShapeEditType::Cone : ShapeEditType::None;
        shapeEditTypeClicked();
    }

    ///////////////////////////////////////////////////////////////////////////

    if (cubeEdit || circleEdit || cylinderEdit || coneEdit)
    {
        if (ImGui::Button("Accept"))
        {
            shapeAcceptClicked();
        }
        ImGui::SameLine();
        if (ImGui::Button("Center"))
        {
            shapeCameraClicked();
        }

        ImGui::Checkbox("Static", &isStatic);

        ImGui::Text("%06.2f %06.2f %06.2f", shapePosition.x, shapePosition.y, shapePosition.z);
        {
            static int32_t e = 0;
            bool clicked = false;
            clicked |= ImGui::RadioButton("translate", &e, 0);
            clicked |= ImGui::RadioButton("rotate", &e, 1);
            clicked |= ImGui::RadioButton("scale", &e, 2);
            switch (e)
            {
            case 0: shapeEditMode = ShapeEditMode::Translate; break;
            case 1: shapeEditMode = ShapeEditMode::Rotate; break;
            case 2: shapeEditMode = ShapeEditMode::Scale; break;
            }
            if (clicked)
                shapeEditModeClicked();
        }

        if (ImGui::CollapsingHeader("Material"))
        {
            bool modified = false;

            modified |= ImGui::ColorEdit3("Ambient", &materialData.ambient[0]);
            modified |= ImGui::ColorEdit3("Diffuse", &materialData.diffuse[0]);
            modified |= ImGui::ColorEdit3("Specular", &materialData.specular[0]);
            modified |= ImGui::SliderFloat("Shininess", &materialData.shininess, 0.0f, 50.0f, "%.4f", 2.0f);

            if (modified && shapeMaterialChanged)
                shapeMaterialChanged();
        }
    }

    ImGui::End();
}
