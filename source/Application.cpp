#include "Application.h"
#include "Common.h"
#include "Shader.h"
#include "Texture.h"
#include "Camera.h"
#include "VboIndexer.h"
#include "Text2D.h"
#include <string>
#include "OpenGL.h"
#include <cmath>
#include <optional>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <inttypes.h>
#include "ObjModel.h"
#include "Light.h"
#include "Model.h"
#include "ModelShader.h"
#include "Skybox.h"
#include "Postprocess.h"
#include "ShadowScene.h"

namespace Application
{
    glm::vec3 g_lightPositionWorldSpace;
    
    CameraRotate g_camera;
    //CameraKeyboard g_camera;

    std::unique_ptr<Postprocess> g_postprocess;
    std::unique_ptr<ObjModel> g_objModel;
    //std::unique_ptr<Model> g_model;
    std::unique_ptr<Light> g_light;
    std::unique_ptr<Skybox> g_skybox;
    std::unique_ptr<ShadowScene> g_shadowScene;

    //void BindModel()
    //{
    //    //Model::Material material;
    //    //material.ambient = { 1.0f, 1.0f, 1.0f };
    //    //material.diffuse = { 1.0f, 0.0f, 0.0f };
    //    //material.specular = { 0.0f, 1.0f, 0.0f };
    //    //material.shininess = 32.0f;

    //    ModelShader::Data data;
    //    data.material.shininess = 10.0f;
    //    data.cameraWorldSpace = g_camera.GetPosition();
    //    data.material.ambient = glm::vec3(0.3f, 0.3f, 0.3f);
    //    data.material.diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
    //    data.material.specular = glm::vec3(0.5f, 0.5f, 0.5f);

    //    data.lightDirectional.direction = glm::vec3(1.0f, -1.0f, 0.0f);
    //    data.lightDirectional.ambient = { 0.1f, 0.1f, 0.1f };
    //    data.lightDirectional.diffuse = { 0.5f, 0.5f, 0.5f };
    //    data.lightDirectional.specular = { 0.3f, 0.3f, 0.3f };

    //    ModelShader::Data::LightPoint lightBlue;
    //    lightBlue.position = g_lightPositionWorldSpace;
    //    lightBlue.constant = 1.0f;
    //    lightBlue.linear = 0.09f;
    //    lightBlue.quadratic = 0.032f;
    //    lightBlue.ambient = { 0.1f, 0.1f, 0.1f };
    //    lightBlue.diffuse = { 0.9f, 0.9f, 0.9f };
    //    lightBlue.specular = { 0.5f, 0.5f, 0.5f };

    //    data.lightPoint.push_back(lightBlue);

    //    ModelShader::Data::LightSpot lightRed;
    //    lightRed.position = g_lightPositionWorldSpace;
    //    lightRed.direction = -g_lightPositionWorldSpace;
    //    lightRed.cutOff = glm::cos(glm::radians(12.5f));
    //    lightRed.outerCutOff = glm::cos(glm::radians(17.5f));
    //    lightRed.ambient = { 0.1f, 0.1f, 0.1f };
    //    lightRed.diffuse = { 1.0f, 0.0f, 0.0f };
    //    lightRed.specular = { 0.5f, 0.5f, 0.5f };
    //    lightRed.constant = 1.0f;
    //    lightRed.linear = 0.09f;
    //    lightRed.quadratic = 0.032f;

    //    data.lightSpot.push_back(lightRed);

    //    ModelShader::Data::LightSpot lightGreen;
    //    lightGreen.position = -g_lightPositionWorldSpace;
    //    lightGreen.direction = g_lightPositionWorldSpace;
    //    lightGreen.cutOff = glm::cos(glm::radians(12.5f));
    //    lightGreen.outerCutOff = glm::cos(glm::radians(17.5f));
    //    lightGreen.ambient = { 0.1f, 0.1f, 0.1f };
    //    lightGreen.diffuse = { 0.0f, 1.0f, 0.0f };
    //    lightGreen.specular = { 0.5f, 0.5f, 0.5f };
    //    lightGreen.constant = 1.0f;
    //    lightGreen.linear = 0.09f;
    //    lightGreen.quadratic = 0.032f;

    //    data.lightSpot.push_back(lightGreen);

    //    g_model->Bind(data);
    //}

    void Draw()
    {
        //g_postprocess->BeginRender();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // drawing single model
        //BindModel();

        // enable blending
        //glEnable(GL_BLEND);
        //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);     

        //glm::mat4 model = glm::mat4(1.0f);
        //g_model->Draw(model, g_camera.GetViewMatrix(), g_camera.GetProjectionMatrix());

        //g_objModel->Draw(glm::mat4(1.0f), g_camera.GetViewMatrix(), g_camera.GetProjectionMatrix(), g_lightPositionWorldSpace, g_camera.GetPosition());
        //g_objModel->DrawSkyboxReflection(g_skybox->GetSkyboxTexture(), glm::mat4(1.0f), g_camera.GetViewMatrix(), g_camera.GetProjectionMatrix(), g_camera.GetPosition());
        //g_objModel->DrawSkyboxRefraction(g_skybox->GetSkyboxTexture(), glm::mat4(1.0f), g_camera.GetViewMatrix(), g_camera.GetProjectionMatrix(), g_camera.GetPosition());

        //glm::mat4 lightModel = glm::mat4(1.0f);
        //lightModel = glm::translate(lightModel, g_lightPositionWorldSpace);
        //lightModel = glm::scale(lightModel, glm::vec3(0.1f)); // a smaller cube
        //g_light->Draw(lightModel, g_camera.GetViewMatrix(), g_camera.GetProjectionMatrix());

        //g_skybox->Draw(g_camera.GetViewMatrix(), g_camera.GetProjectionMatrix());

        g_shadowScene->Draw(g_camera.GetViewMatrix(), g_camera.GetProjectionMatrix(), g_camera.GetPosition());

        //glDisable(GL_BLEND);

        //g_postprocess->EndRender();
    }

    void RecomputeMVPMatrix()
    {
        //g_camera.RecomputeMatrices();
    }

    bool Init()
    {
        if (!InitOpenGL())
        {
            printf("Error initializing OpenGl\n");
            return false;
        }

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
        glEnable(GL_MULTISAMPLE);

        // Enable gamma correction for all framebuffers, this is maybe not good idea.
        glEnable(GL_FRAMEBUFFER_SRGB);

        g_camera.Init();

        g_lightPositionWorldSpace = glm::vec3(5.0f, 0.0f, 0.0f);

        //if (!Text2DInitFont())
        //{
        //    printf("Error initializing font.\n");
        //    return false;
        //}

        //g_model.reset(new Model("models/moses/scene.model", light));
        //g_model.reset(new Model("models/skull/craneo.model", light));
        //g_model.reset(new Model("models/cube/cube.model", light));
        //g_objModel.reset(new ObjModel());
        //g_light.reset(new Light());

        //g_skybox.reset(new Skybox({ "skybox/islands/right.jpg",
        //                            "skybox/islands/left.jpg",
        //                            "skybox/islands/top.jpg",
        //                            "skybox/islands/bottom.jpg",
        //                            "skybox/islands/front.jpg",
        //                            "skybox/islands/back.jpg" }));

        //g_postprocess.reset(new Postprocess(Postprocess::Type::KernelSharpen));

        g_shadowScene.reset(new ShadowScene());

        return true;
    }

    bool MainLoop()
    {
        RecomputeMVPMatrix();

        // rotate light
        //glm::mat4 rotationMatrix(1.0f);
        //rotationMatrix = glm::rotate(rotationMatrix, glm::radians(0.1f), glm::vec3(0.0, 1.0, 0.0));
        //g_lightPositionWorldSpace = glm::vec3(rotationMatrix * glm::vec4(g_lightPositionWorldSpace, 1.0));

        Draw();

        //Text2DPrint("Hello World.", 100, 100, 40);

        return true;
    }

    void Dispatch(const SDL_Event & event)
    {
        // touch events
        if (event.type == SDL_FINGERDOWN)
        {
            auto[width, height] = Common::GetWindowSize();
            g_camera.Press({ event.tfinger.x * (float)width, event.tfinger.y * (float)height }, event.tfinger.fingerId);
        }
        else if (event.type == SDL_FINGERUP)
        {
            auto[width, height] = Common::GetWindowSize();
            g_camera.Release({ event.tfinger.x * (float)width, event.tfinger.y * (float)height }, event.tfinger.fingerId);
        }
        else if (event.type == SDL_FINGERMOTION)
        {
            auto[width, height] = Common::GetWindowSize();
            g_camera.Move({ event.tfinger.x * (float)width, event.tfinger.y * (float)height }, event.tfinger.fingerId);
        }
#if !defined(__ANDROID__)
        // looks like mouse events are sent also on android for some reason
        else if (event.type == SDL_MOUSEWHEEL)
        {
            //g_camera.ModifyFoV(event.wheel.y);
            g_camera.Wheel((float)event.wheel.y);
        }
        else if (event.type == SDL_MOUSEBUTTONDOWN)
        {
            g_camera.Press({ (float)event.button.x, (float)event.button.y }, 0);
        }
        else if (event.type == SDL_MOUSEBUTTONUP)
        {
            g_camera.Release({ (float)event.button.x, (float)event.button.y }, 0);
        }
        else if (event.type == SDL_MOUSEMOTION)
        {
            g_camera.Move({ (float)event.motion.x, (float)event.motion.y }, 0);
        }
#endif
    }

    void Deinit()
    {
        //Text2DCleanup();
    }
}
