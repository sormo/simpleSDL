#include "UserInterface.h"
#include "Common.h"
#include <SDL.h>

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

    if (cubeEdit || circleEdit || cylinderEdit || coneEdit)
    {
        if (ImGui::Button("Accept"))
        {
            shapeAcceptClicked();
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

        if (shapeEditMode == ShapeEditMode::Translate || shapeEditMode == ShapeEditMode::Scale)
        {
            static int a = 0;
            ImGui::RadioButton("XY", &a, 0); ImGui::SameLine();
            ImGui::RadioButton("XZ", &a, 1); ImGui::SameLine();
            ImGui::RadioButton("YZ", &a, 2);
            switch (a)
            {
            case 0: shapeEditAxis = ShapeEditAxis::XY; break;
            case 1: shapeEditAxis = ShapeEditAxis::XZ; break;
            case 2: shapeEditAxis = ShapeEditAxis::YZ; break;
            }
        }
    }

    ImGui::End();
}
