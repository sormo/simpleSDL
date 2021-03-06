#include "ApplicationMain.h"
#include "Common.h"
#include "utils/Shader.h"
#include "utils/Texture.h"
#include "utils/Camera.h"
#include "VboIndexer.h"
#include "legacy/Text2D.h"
#include <string>
#include "OpenGL.h"
#include <cmath>
#include <optional>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <inttypes.h>
#include "legacy/ObjModel.h"
#include "legacy/LightObject.h"
#include "model/Model.h"
#include "model/ModelShader.h"
#include "utils/Skybox.h"
#include "utils/Postprocess.h"
#include "legacy/ShadowScene.h"
#include "scene/Scene.h"
#include "editor/UserInterface.h"
#include "EventDispatchers.h"
#include "editor/Editor.h"

#include "DebugDraw.h"

namespace
{
    glm::vec3 g_lightPositionWorldSpace;

    CameraRotate g_camera;
    //CameraKeyboard g_camera;

    std::unique_ptr<UserInterface> g_userInterface;
    MouseDispatcher g_mouseDispatcher;
    ResizeDispatcher g_resizeDispatcher;

    std::unique_ptr<Postprocess> g_postprocess;
    std::unique_ptr<ObjModel> g_objModel;
    std::unique_ptr<Model> g_model;
    std::unique_ptr<LightObject> g_light;
    std::unique_ptr<Skybox> g_skybox;
    std::unique_ptr<ShadowScene> g_shadowScene;
    std::unique_ptr<Scene> g_scene;
    std::unique_ptr<Editor> g_editor;
    std::unique_ptr<DebugDraw> g_debugDraw;

    void GenerateBoxes()
    {
        Material::Data material;
        material.ambient = material.diffuse = material.specular = glm::vec3(1.0, 1.0, 1.0);
        material.shininess = 15.0f;

        g_scene->AddCube({ glm::vec3(0.0f, -2.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(20.0f, 0.5f, 20.0f) }, material, true);
        g_scene->AddCube({ glm::vec3(0.0f, 1.5f, 0.0), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.5f) }, material, true);
        g_scene->AddCube({ glm::vec3(2.0f, 0.0f, 1.0), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.5f) }, material, true);
        g_scene->AddCube({ glm::vec3(-1.0f, 0.0f, 2.0), Common::Math::GetRotation(glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0))), glm::vec3(0.25) }, material, true);
    }

    void InitScene()
    {
        Light::Config light;
        light.directional = true;
        light.spotCount = 0;
        light.pointCount = 0;

        g_scene = std::make_unique<Scene>(light);

        GenerateBoxes();
    }

    void DrawScene()
    {
        g_userInterface->lightData.lightDirectional.direction = -g_lightPositionWorldSpace;

        //Light::Data::LightSpot spotLight;
        //spotLight.position = g_lightPositionWorldSpace;
        //spotLight.direction = -g_lightPositionWorldSpace;
        //spotLight.cutOff = glm::cos(glm::radians(60.0f));
        //spotLight.outerCutOff = glm::cos(glm::radians(70.0f));
        //spotLight.ambient = { 0.1f, 0.1f, 0.1f };
        //spotLight.diffuse = { 1.0f, 1.0f, 1.0f };
        //spotLight.specular = { 0.5f, 0.5f, 0.5f };
        //spotLight.constant = 1.0f;
        //spotLight.linear = 0.09f;
        //spotLight.quadratic = 0.032f;
        //light.lightSpot.push_back(spotLight);

        //Light::Data::LightPoint pointLight;
        //pointLight.position = g_lightPositionWorldSpace;
        //pointLight.ambient = { 0.1f, 0.1f, 0.1f };
        //pointLight.diffuse = { 1.0f, 1.0f, 1.0f };
        //pointLight.specular = { 0.5f, 0.5f, 0.5f };
        //pointLight.constant = 1.0f;
        //pointLight.linear = 0.09f;
        //pointLight.quadratic = 0.032f;
        //light.lightPoint.push_back(pointLight);

        g_scene->Step();

        g_scene->Draw(g_camera.GetViewMatrix(), g_camera.GetProjectionMatrix(), g_camera.GetPosition(), g_userInterface->lightData);

        if (g_userInterface->bulletDebug)
            g_scene->DrawDebug(g_camera.GetViewMatrix(), g_camera.GetProjectionMatrix());

        g_editor->Draw(g_camera.GetViewMatrix(), g_camera.GetProjectionMatrix());
    }

    void DrawTestDebugDraw()
    {
        for (float i = 1.0f; i <= 10.0f; i += 1.0f)
        {
            g_debugDraw->DrawPoint({ i, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f });
            g_debugDraw->DrawPoint({ 0.0f, i, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f });
            g_debugDraw->DrawPoint({ 0.0f, 0.0f, i }, { 0.0f, 0.0f, 1.0f, 1.0f });
        }

        g_debugDraw->DrawLine({ 0.0f, 0.0f, 0.0f }, { 10.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f });
        g_debugDraw->DrawLine({ 0.0f, 0.0f, 0.0f }, { 0.0f, 10.0f, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f });
        g_debugDraw->DrawLine({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 10.0f }, { 0.0f, 0.0f, 1.0f, 1.0f });

        g_debugDraw->DrawTriangle({ 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 0.5f });

        Common::Math::Plane plane = Common::Math::Plane::CreateFromPoints({ 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f });
        g_debugDraw->DrawRectangle({ 0.0f, 0.0f, 0.0f }, { 0.5f, 1.0f }, plane.normal, { 0.0f, 1.0f, 0.0f, 0.5f });

        plane.Translate({ 0.0f, 2.0f, 0.0f });
        g_debugDraw->DrawPlane(plane, { 1.0f, 1.0f }, { 0.0f, 1.0f, 0.0f, 0.2f });

        Common::Math::Plane plane2 = Common::Math::Plane::CreateFromPoints({ 1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 0.0f }, { 1.0f, 0.0f, 1.0f });

        //g_debugDraw->DrawPlane(plane2, { 1.0f, 1.0f }, { 1.0f, 0.0f, 0.0f, 0.5f });

        plane2.Rotate(Common::Math::GetRotation(glm::radians(45.0f), { 0.0f, 0.0f, 1.0f }));
        plane2.Position({ 1.0f, 0.0f, 0.0f });

        g_debugDraw->DrawPlane(plane2, { 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 0.5f });

        Common::Math::Line line = Common::Math::Line::CreateFromPoints({ -1.0f, 1.0f, 1.0f }, { -0.5f, -0.5f, 0.0f });
        g_debugDraw->DrawLine(line, 5.0f, { 1.0f, 1.0f, 1.0f, 0.5f });

        glm::vec3 intersection = Common::Math::GetIntersection(plane2, line);
        g_debugDraw->DrawPoint(intersection, { 1.0f, 0.0f, 0.0f, 1.0f });

        g_debugDraw->Draw(g_camera.GetViewMatrix(), g_camera.GetProjectionMatrix());
        g_debugDraw->Clear();
    }

    void BindModel()
    {
        Model::Data data;

        data.cameraWorldSpace = g_camera.GetPosition();

        data.material.shininess = 10.0f;
        data.material.ambient = glm::vec3(0.3f, 0.3f, 0.3f);
        data.material.diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
        data.material.specular = glm::vec3(0.5f, 0.5f, 0.5f);

        data.light.lightDirectional.direction = glm::vec3(1.0f, -1.0f, 0.0f);
        data.light.lightDirectional.ambient = { 0.1f, 0.1f, 0.1f };
        data.light.lightDirectional.diffuse = { 0.5f, 0.5f, 0.5f };
        data.light.lightDirectional.specular = { 0.3f, 0.3f, 0.3f };

        Light::Data::LightPoint pointLight;
        pointLight.position = g_lightPositionWorldSpace;
        pointLight.constant = 1.0f;
        pointLight.linear = 0.09f;
        pointLight.quadratic = 0.032f;
        pointLight.ambient = { 0.1f, 0.1f, 0.1f };
        pointLight.diffuse = { 0.9f, 0.9f, 0.9f };
        pointLight.specular = { 0.5f, 0.5f, 0.5f };

        data.light.lightPoint.push_back(pointLight);

        Light::Data::LightSpot spotLight;
        spotLight.position = g_lightPositionWorldSpace;
        spotLight.direction = -g_lightPositionWorldSpace;
        spotLight.cutOff = glm::cos(glm::radians(12.5f));
        spotLight.outerCutOff = glm::cos(glm::radians(17.5f));
        spotLight.ambient = { 0.1f, 0.1f, 0.1f };
        spotLight.diffuse = { 1.0f, 0.0f, 0.0f };
        spotLight.specular = { 0.5f, 0.5f, 0.5f };
        spotLight.constant = 1.0f;
        spotLight.linear = 0.09f;
        spotLight.quadratic = 0.032f;

        data.light.lightSpot.push_back(spotLight);

        g_model->Bind(data);
    }

    void Draw()
    {
        glViewport(0, 0, Common::GetWindowWidth(), Common::GetWindowHeight());
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //g_postprocess->BeginRender();

#if !defined(EMSCRIPTEN) && !defined(ANDROID)
        if (g_userInterface->wireframe)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif

        // enable blending
        //glEnable(GL_BLEND);
        //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);     

        DrawScene();
        //DrawTestDebugDraw();

        // drawing single model
        //BindModel();
        //g_model->Draw(glm::mat4(1.0f), g_camera.GetViewMatrix(), g_camera.GetProjectionMatrix());

        //g_objModel->Draw(glm::mat4(1.0f), g_camera.GetViewMatrix(), g_camera.GetProjectionMatrix(), g_lightPositionWorldSpace, g_camera.GetPosition());
        //g_objModel->DrawSkyboxReflection(g_skybox->GetSkyboxTexture(), glm::mat4(1.0f), g_camera.GetViewMatrix(), g_camera.GetProjectionMatrix(), g_camera.GetPosition());
        //g_objModel->DrawSkyboxRefraction(g_skybox->GetSkyboxTexture(), glm::mat4(1.0f), g_camera.GetViewMatrix(), g_camera.GetProjectionMatrix(), g_camera.GetPosition());

        //glm::mat4 lightModel = glm::mat4(1.0f);
        //lightModel = glm::translate(lightModel, g_lightPositionWorldSpace);
        //lightModel = glm::scale(lightModel, glm::vec3(0.1f)); // a smaller cube
        //g_light->Draw(lightModel, g_camera.GetViewMatrix(), g_camera.GetProjectionMatrix());

        //g_skybox->Draw(g_camera.GetViewMatrix(), g_camera.GetProjectionMatrix());

        //g_shadowScene->Draw(g_camera.GetViewMatrix(), g_camera.GetProjectionMatrix(), g_camera.GetPosition(), g_lightPositionWorldSpace, -g_lightPositionWorldSpace, glm::cos(glm::radians(75.0f)), glm::cos(glm::radians(80.0f)));

        //glDisable(GL_BLEND);

        //g_postprocess->EndRender();

#if !defined(EMSCRIPTEN) && !defined(ANDROID)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif
    }

    void RecomputeMVPMatrix()
    {
        //g_camera.RecomputeMatrices();
    }
}
bool ApplicationMain::Init()
{
    PrintAllExtensions();

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

    //if (!Text2DInitFont())
    //{
    //    printf("Error initializing font.\n");
    //    return false;
    //}

    InitScene();

    //Light::Config light;
    //light.directional = true;
    //light.spotCount = 1;
    //g_model.reset(new Model("models/moses/scene.model", light));
    //g_model.reset(new Model("models/craneo/craneo.model", light));
    //g_model.reset(new Model("models/cube/cube.model", light));
    //g_objModel.reset(new ObjModel());
    //g_light.reset(new LightObject());

    //g_skybox.reset(new Skybox({ "skybox/islands/right.jpg",
    //                            "skybox/islands/left.jpg",
    //                            "skybox/islands/top.jpg",
    //                            "skybox/islands/bottom.jpg",
    //                            "skybox/islands/front.jpg",
    //                            "skybox/islands/back.jpg" }));

    //g_postprocess.reset(new Postprocess(Postprocess::Type::KernelSharpen));

    g_shadowScene.reset(new ShadowScene());

    g_userInterface.reset(new UserInterface(g_camera));

    g_editor.reset(new Editor(*g_scene, *g_userInterface, g_camera));

    g_debugDraw.reset(new DebugDraw());

    g_mouseDispatcher.Add(g_editor.get());
    g_mouseDispatcher.Add(&g_camera);

    g_resizeDispatcher.Add(&g_camera);

#ifdef ANDROID
    ImGui::GetStyle().ScaleAllSizes(4.0f);
#endif

    return true;
}

bool ApplicationMain::MainLoop()
{
    RecomputeMVPMatrix();

    // rotate light
    glm::mat4 rotationMatrix(1.0f);
    rotationMatrix = glm::rotate(rotationMatrix, glm::radians(0.1f), glm::vec3(0.0, 1.0, 0.0));
    g_lightPositionWorldSpace = glm::vec3(rotationMatrix * glm::vec4(g_lightPositionWorldSpace, 1.0));

    Draw();

    //Text2DPrint("Hello World.", 100, 100, 40);

    return true;
}

void ApplicationMain::Dispatch(const SDL_Event & event)
{
    if (g_mouseDispatcher.Dispatch(event))
        return;

    if (g_resizeDispatcher.Dispatch(event))
        return;
}

void ApplicationMain::Deinit()
{
    //Text2DCleanup();
}

void ApplicationMain::Gui()
{
    g_userInterface->Generate();
}
