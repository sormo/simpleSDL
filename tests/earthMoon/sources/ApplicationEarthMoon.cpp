#include "ApplicationEarthMoon.h"

#include "Common.h"
#include "utils/Shader.h"
#include "utils/Texture.h"
#include "utils/Camera.h"
#include <string>
#include <cmath>
#include <optional>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "model/Model.h"
#include "model/ModelShader.h"
#include "scene/Scene.h"
#include "EventDispatchers.h"

namespace
{
    glm::vec3 g_lightPositionWorldSpace;

    CameraRotate g_camera;
    MouseDispatcher g_mouseDispatcher;
    ResizeDispatcher g_resizeDispatcher;
    std::unique_ptr<Scene> g_scene;
    Light::Data g_lightData;

    void GenerateEarthMoon()
    {
        Material::Data material;
        material.diffuse = material.specular = glm::vec3(1.0, 1.0, 1.0);
        material.ambient = glm::vec3(0.2f, 0.2f, 0.2f);
        material.shininess = 50.0f;

        Shapes::Defintion::Sphere moon;

        moon.position = glm::vec3(0.0f, 384.4f, 0.0f);
        moon.radius = 1.7371f;

        g_scene->AddSphere(moon, material, true);

        Shapes::Defintion::Sphere earth;

        earth.position = glm::vec3(0.0f, 0.0f, 0.0f);
        earth.radius = 6.371f;

        g_scene->AddSphere(earth, material, true);
    }

    void InitScene()
    {
        Light::Config light;
        light.directional = true;
        light.spotCount = 0;
        light.pointCount = 0;

        g_lightData.lightDirectional.ambient = glm::vec3(0.1f, 0.1f, 0.1f);
        g_lightData.lightDirectional.diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
        g_lightData.lightDirectional.specular = glm::vec3(0.1f, 0.1f, 0.1f);

        g_scene = std::make_unique<Scene>(light);

        GenerateEarthMoon();
    }

    void DrawScene()
    {
        g_scene->Step();

        g_lightData.lightDirectional.direction = -g_lightPositionWorldSpace;

        g_scene->Draw(g_camera.GetViewMatrix(), g_camera.GetProjectionMatrix(), g_camera.GetPosition(), g_lightData);
    }

    void Draw()
    {
        glViewport(0, 0, Common::GetWindowWidth(), Common::GetWindowHeight());
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        DrawScene();
    }
}

bool ApplicationEarthMoon::Init()
{
    glClearColor(0, 0, 0, 0);
    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);
    // Enable backface culling.
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    // Multisampling are configured in main before window creation.
    //glEnable(GL_MULTISAMPLE);

    // Enable gamma correction for all framebuffers, this is maybe not good idea.
    //glEnable(GL_FRAMEBUFFER_SRGB);

    g_camera.Init();

    g_lightPositionWorldSpace = glm::vec3(30.0f, 25.0f, 0.0f);

    InitScene();

    g_mouseDispatcher.Add(&g_camera);
    g_resizeDispatcher.Add(&g_camera);

#ifdef ANDROID
    ImGui::GetStyle().ScaleAllSizes(4.0f);
#endif

    return true;
}

bool ApplicationEarthMoon::MainLoop()
{
    // rotate light
    glm::mat4 rotationMatrix(1.0f);
    rotationMatrix = glm::rotate(rotationMatrix, glm::radians(0.1f), glm::vec3(0.0, 1.0, 0.0));
    g_lightPositionWorldSpace = glm::vec3(rotationMatrix * glm::vec4(g_lightPositionWorldSpace, 1.0));

    Draw();

    return true;
}

void ApplicationEarthMoon::Dispatch(const SDL_Event & event)
{
    if (g_mouseDispatcher.Dispatch(event))
        return;

    if (g_resizeDispatcher.Dispatch(event))
        return;
}

void ApplicationEarthMoon::Deinit()
{

}

void ApplicationEarthMoon::Gui()
{

}
