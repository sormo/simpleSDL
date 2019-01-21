#include "Application.h"
#include "Common.h"
#include "Shader.h"
#include "Texture.h"
#include "Camera.h"
#include "ObjLoader.h"
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

namespace Application
{
    glm::vec3 g_lightPositionWorldSpace;
    
    CameraRotate g_camera;

    std::unique_ptr<Cylinder> g_cylinder;
    std::unique_ptr<Light> g_light;

    void DrawModel()
    {
        // enable blending
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        g_cylinder->Draw(glm::mat4(1.0f), g_camera.GetViewMatrix(), g_camera.GetProjectionMatrix(), g_lightPositionWorldSpace);

        glm::mat4 lightModel = glm::mat4(1.0f);
        lightModel = glm::translate(lightModel, g_lightPositionWorldSpace);
        lightModel = glm::scale(lightModel, glm::vec3(0.1f)); // a smaller cube

        g_light->Draw(lightModel, g_camera.GetViewMatrix(), g_camera.GetProjectionMatrix());

        glDisable(GL_BLEND);
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

        glEnable(GL_CULL_FACE);

        g_camera.Init();

        g_lightPositionWorldSpace = glm::vec3(4.0f, 0.0f, 0.0f);

        if (!Text2DInitFont("text2d-32bpp.bmp"))
        {
            printf("Error initializing font.\n");
            return false;
        }

        g_cylinder.reset(new Cylinder());
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
        rotationMatrix = glm::rotate(rotationMatrix, glm::radians(1.0f), glm::vec3(0.0, 1.0, 0.0));
        g_lightPositionWorldSpace = glm::vec3(rotationMatrix * glm::vec4(g_lightPositionWorldSpace, 1.0));

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        DrawModel();

        Text2DPrint("Hello World.", 100, 100, 40);

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
        Text2DCleanup();
    }
}
