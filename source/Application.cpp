#include "Application.h"
#include "Common.h"
#include "Shader.h"
#include "Texture.h"
#include "Camera.h"
#include "ModelLoader.h"
#include "VboIndexer.h"
#include "Text2D.h"
#include <string>
#include "OpenGL.h"
#include <cmath>
#include <optional>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <inttypes.h>
#include "Cylinder.h"
#include "Light.h"
#include "Model.h"
#include "ModelShader.h"

namespace Application
{
    glm::vec3 g_lightPositionWorldSpace;
    
    CameraRotate g_camera;
    //CameraKeyboard g_camera;

    //std::unique_ptr<Cylinder> g_cylinder;
    std::unique_ptr<Model> g_model;
    std::shared_ptr<ModelShader> g_modelShader;
    std::unique_ptr<Light> g_light;

    // positions all containers
    glm::vec3 g_cubePositions[] = {
        glm::vec3(0.0f,  0.0f,  0.0f),
        glm::vec3(2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f,  2.0f, -2.5f),
        glm::vec3(1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };

    void BindModel()
    {
        //Model::Material material;
        //material.ambient = { 1.0f, 1.0f, 1.0f };
        //material.diffuse = { 1.0f, 0.0f, 0.0f };
        //material.specular = { 0.0f, 1.0f, 0.0f };
        //material.shininess = 32.0f;

        ModelShader::Data data;
        data.shininess = 10.0f;
        data.cameraWorldSpace = g_camera.GetPosition();
        data.material.ambient = glm::vec3(0.3f, 0.3f, 0.3f);
        data.material.diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
        data.material.specular = glm::vec3(0.5f, 0.5f, 0.5f);

        data.lightDirectional.direction = glm::vec3(1.0f, -1.0f, 0.0f);
        data.lightDirectional.ambient = { 0.1f, 0.1f, 0.1f };
        data.lightDirectional.diffuse = { 0.5f, 0.5f, 0.5f };
        data.lightDirectional.specular = { 0.3f, 0.3f, 0.3f };

        ModelShader::Data::LightPoint lightBlue;
        lightBlue.position = g_lightPositionWorldSpace;
        lightBlue.constant = 1.0f;
        lightBlue.linear = 0.09f;
        lightBlue.quadratic = 0.032f;
        lightBlue.ambient = { 0.1f, 0.1f, 0.1f };
        lightBlue.diffuse = { 0.5f, 0.5f, 0.5f };
        lightBlue.specular = { 0.5f, 0.5f, 0.5f };

        data.lightPoint.push_back(lightBlue);

        ModelShader::Data::LightSpot lightRed;
        lightRed.position = g_lightPositionWorldSpace;
        lightRed.direction = -g_lightPositionWorldSpace;
        lightRed.cutOff = glm::cos(glm::radians(12.5f));
        lightRed.outerCutOff = glm::cos(glm::radians(17.5f));
        lightRed.ambient = { 0.1f, 0.1f, 0.1f };
        lightRed.diffuse = { 1.0f, 0.0f, 0.0f };
        lightRed.specular = { 0.5f, 0.5f, 0.5f };
        lightRed.constant = 1.0f;
        lightRed.linear = 0.09f;
        lightRed.quadratic = 0.032f;

        data.lightSpot.push_back(lightRed);

        ModelShader::Data::LightSpot lightGreen;
        lightGreen.position = -g_lightPositionWorldSpace;
        lightGreen.direction = g_lightPositionWorldSpace;
        lightGreen.cutOff = glm::cos(glm::radians(12.5f));
        lightGreen.outerCutOff = glm::cos(glm::radians(17.5f));
        lightGreen.ambient = { 0.1f, 0.1f, 0.1f };
        lightGreen.diffuse = { 0.0f, 1.0f, 0.0f };
        lightGreen.specular = { 0.5f, 0.5f, 0.5f };
        lightGreen.constant = 1.0f;
        lightGreen.linear = 0.09f;
        lightGreen.quadratic = 0.032f;

        data.lightSpot.push_back(lightGreen);

        g_model->Bind(data);
    }

    void Draw()
    {
        // drawing single model
        BindModel();

        // enable blending
        //glEnable(GL_BLEND);
        //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);     

        for (uint32_t i = 0; i < 10; i++)
        {
            // calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, g_cubePositions[i]);
            float angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));

            g_model->Draw(model, g_camera.GetViewMatrix(), g_camera.GetProjectionMatrix());
        }

        //g_cylinder->Draw(glm::mat4(1.0f), g_camera.GetViewMatrix(), g_camera.GetProjectionMatrix(), g_lightPositionWorldSpace);

        glm::mat4 lightModel = glm::mat4(1.0f);
        lightModel = glm::translate(lightModel, g_lightPositionWorldSpace);
        lightModel = glm::scale(lightModel, glm::vec3(0.1f)); // a smaller cube
        g_light->Draw(lightModel, g_camera.GetViewMatrix(), g_camera.GetProjectionMatrix());

        //glDisable(GL_BLEND);
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
            PrintOpenGlPointers();
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

        g_camera.Init();

        g_lightPositionWorldSpace = glm::vec3(5.0f, 0.0f, 0.0f);

        //if (!Text2DInitFont())
        //{
        //    printf("Error initializing font.\n");
        //    return false;
        //}

        ModelShader::Config modelConfig;
        modelConfig.textureDiffuseCount = 1;
        modelConfig.textureNormalCount = 1;
        modelConfig.lightDirection = true;
        modelConfig.lightPointCount = 1;
        modelConfig.lightSpotCount = 2;

        //g_cylinder.reset(new Cylinder());
        //g_modelShader.reset(new ModelShader("shaders/material/vertColorMapNew.glsl", "shaders/material/fragColorMapSpotMultiple.glsl", modelConfig));
        g_modelShader.reset(new ModelShader("shaders/material/vertColorMapNewNormal.glsl", "shaders/material/fragColorMapSpotMultipleNormal.glsl", modelConfig));
        if (!g_modelShader->GetShader())
        {
            printf("Error loading shaders.\n");
            return false;
        }

        //g_model.reset(new Model("models/nanosuit/nanosuit.model", Model::FlagTextureDiffuse | Model::FlagNormal | Model::FlagTextureNormal | Model::FlagTextureSpecular));
        g_model.reset(new Model("models/cube2/cube2.model", g_modelShader));
        //g_model.reset(new Model("models/rubix/RubixCube.model", g_modelShader));
        g_light.reset(new Light());

        return true;
    }

    void PrintMillisecondsPerFrame()
    {
        static double lastTime = Common::GetCurrentTimeInSeconds();
        static uint32_t numberOfFrames = 0;

        double currentTime = Common::GetCurrentTimeInSeconds();
        numberOfFrames++;

        if (currentTime - lastTime >= 1.0)
        {
            printf("%f ms/frame\n", 1000.0 / (double)numberOfFrames);
            numberOfFrames = 0;
            lastTime += 1.0;
        }
    }

    bool MainLoop()
    {
        //PrintMillisecondsPerFrame();

        RecomputeMVPMatrix();

        // rotate light
        glm::mat4 rotationMatrix(1.0f);
        rotationMatrix = glm::rotate(rotationMatrix, glm::radians(0.1f), glm::vec3(0.0, 1.0, 0.0));
        g_lightPositionWorldSpace = glm::vec3(rotationMatrix * glm::vec4(g_lightPositionWorldSpace, 1.0));

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
