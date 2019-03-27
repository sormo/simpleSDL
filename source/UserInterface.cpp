#include "UserInterface.h"
#include "Common.h"
#include <SDL.h>

static const ImVec2 WINDOW_SIZE_DEFAULT(160.0f, 80.0);
static const ImVec2 WINDOW_SIZE_CUBE_EDIT(160.0f, 160.0f);

UserInterface::UserInterface()
    : windowSize(WINDOW_SIZE_DEFAULT)
{

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
    ImGui::SetNextWindowSize(windowSize, ImGuiCond_Always);

    ImGui::Begin("test", nullptr, windowFlags);

    ImGui::Text("Frame: %06.2fms", Common::Frame::GetFPS());
    
    int32_t mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    ImGui::Text("Screen: %03d:%03d", mouseX, mouseY);

    if (ToggleButton("Cube", &cubeEdit))
    {
        cubeEditClicked();
        if (cubeEdit)
            windowSize = WINDOW_SIZE_CUBE_EDIT;
        else
            windowSize = WINDOW_SIZE_DEFAULT;
    }

    if (cubeEdit)
    {
        ImGui::SameLine();
        if (ImGui::Button("Accept"))
        {
            cubeAcceptClicked();
        }

        ImGui::Text("%06.2f %06.2f %06.2f", cubePosition.x, cubePosition.y, cubePosition.z);

        static int e = 0;
        ImGui::RadioButton("position", &e, 0);
        ImGui::RadioButton("distance", &e, 1);
        ImGui::RadioButton("camera", &e, 2);
        switch (e)
        {
        case 0: cubeEditMode = CubeEditMode::Position; break;
        case 1: cubeEditMode = CubeEditMode::Distance; break;
        case 2: cubeEditMode = CubeEditMode::Camera; break;
        }
    }

    ImGui::End();
}
