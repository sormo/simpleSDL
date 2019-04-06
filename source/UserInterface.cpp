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

    if (ToggleButton("Cube", &cubeEdit))
    {
        cubeEditClicked();
    }

    if (cubeEdit)
    {
        ImGui::SameLine();
        if (ImGui::Button("Accept"))
        {
            cubeAcceptClicked();
        }

        ImGui::Text("%06.2f %06.2f %06.2f", cubePosition.x, cubePosition.y, cubePosition.z);
        {
            static int32_t e = 0;
            ImGui::RadioButton("translate", &e, 0);
            ImGui::RadioButton("rotate", &e, 1);
            ImGui::RadioButton("scale", &e, 2);
            ImGui::RadioButton("camera", &e, 3);
            switch (e)
            {
            case 0: cubeEditMode = CubeEditMode::Translate; break;
            case 1: cubeEditMode = CubeEditMode::Rotate; break;
            case 2: cubeEditMode = CubeEditMode::Scale; break;
            case 3: cubeEditMode = CubeEditMode::Camera; break;
            }
        }

        if (cubeEditMode == CubeEditMode::Translate || cubeEditMode == CubeEditMode::Scale)
        {
            static int a = 0;
            ImGui::RadioButton("XY", &a, 0); ImGui::SameLine();
            ImGui::RadioButton("XZ", &a, 1); ImGui::SameLine();
            ImGui::RadioButton("YZ", &a, 2);
            switch (a)
            {
            case 0: cubeEditAxis = CubeEditAxis::XY; break;
            case 1: cubeEditAxis = CubeEditAxis::XZ; break;
            case 2: cubeEditAxis = CubeEditAxis::YZ; break;
            }
        }
    }

    ImGui::End();
}
