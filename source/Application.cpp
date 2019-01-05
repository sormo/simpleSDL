#include "Application.h"
#include "Common.h"
#include "Shader.h"
#include "Texture.h"
#include "Camera.h"
#include "ObjLoader.h"
#include <string>
#include "OpenGL.h"
#include <cmath>
#include <optional>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <inttypes.h>

namespace Application
{
    GLuint g_program;

    GLuint g_locationPosition;
    GLuint g_locationUV;
    GLuint g_locationMVP;
    GLuint g_locationTexture;

    GLuint g_bufferVertexCube;
    GLuint g_bufferUVCube;

    glm::mat4 g_MVPCube;

    GLuint g_verticesCount;

    GLuint g_texture;

    CameraRotate g_camera;

    void InitLocations()
    {
        g_locationPosition = glGetAttribLocation(g_program, "position");
        g_locationUV = glGetAttribLocation(g_program, "vertexUV");
        g_locationMVP = glGetUniformLocation(g_program, "MVP");
        g_locationTexture = glGetUniformLocation(g_program, "textureValue");
    }

    // return number of vertices initialized
    uint32_t InitBuffers()
    {
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec2> uvs;
        std::vector<glm::vec3> normals; // Won't be used at the moment.

        if (!LoadObj("cube.obj", vertices, uvs, normals))
        {
            printf("Error loading cube.obj.");
            return 0;
        }

        glGenBuffers(1, &g_bufferVertexCube);
        glBindBuffer(GL_ARRAY_BUFFER, g_bufferVertexCube);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

        glGenBuffers(1, &g_bufferUVCube);
        glBindBuffer(GL_ARRAY_BUFFER, g_bufferUVCube);
        glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);

        return vertices.size();
    }

    void DrawCube()
    {
        glUniformMatrix4fv(g_locationMVP, 1, GL_FALSE, &g_MVPCube[0][0]);
        
        // Bind our texture in Texture Unit 0
        glActiveTexture(GL_TEXTURE0);

        glBindTexture(GL_TEXTURE_2D, g_texture);

        // Set our "myTextureSampler" sampler to user Texture Unit 0
        glUniform1i(g_locationTexture, 0);

        glEnableVertexAttribArray(g_locationPosition);

        glBindBuffer(GL_ARRAY_BUFFER, g_bufferVertexCube);

        glVertexAttribPointer(g_locationPosition, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        glEnableVertexAttribArray(g_locationUV);

        glBindBuffer(GL_ARRAY_BUFFER, g_bufferUVCube);

        glVertexAttribPointer(g_locationUV, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

        glDrawArrays(GL_TRIANGLES, 0, g_verticesCount);
 
        glDisableVertexAttribArray(g_locationPosition);
        glDisableVertexAttribArray(g_locationUV);
    }

    void RecomputeMVPMatrix()
    {
        //g_camera.RecomputeMatrices();

        glm::mat4 projection = g_camera.GetProjectionMatrix();
        glm::mat4 view = g_camera.GetViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);
        // Our ModelViewProjection : multiplication of our 3 matrices
        g_MVPCube = projection * view * model; // Remember, matrix multiplication is the other way around
    }

    bool Init()
    {
        if (!InitOpenGL())
        {
            printf("Error initializing OpenGl\n");
            PrintOpenGlPointers();
            return false;
        }

        auto program = CreateProgram("shaderVertex.glsl", "shaderFragment.glsl");
        if (!program)
            return false;

        g_program = *program;

        glClearColor(0,0,0,0);
        // Enable depth test
        glEnable(GL_DEPTH_TEST);
        // Accept fragment if it closer to the camera than the former one
        glDepthFunc(GL_LESS);

        InitLocations();
        g_verticesCount = InitBuffers();
        if (g_verticesCount == 0)
            return false;

        // Load the texture using any two methods
        auto texture = LoadBMP("cube.bmp");
        //auto texture = LoadDDS("cube.dds");
        if (!texture)
        {
            printf("Error loading texture.\n");
            return false;
        }
        g_texture = *texture;

        g_camera.Init();

        return true;
    }

    bool MainLoop()
    {
        RecomputeMVPMatrix();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(g_program);

        DrawCube();

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
            g_camera.Wheel(event.wheel.y);
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
        glDeleteBuffers(1, &g_bufferVertexCube);
        glDeleteBuffers(1, &g_bufferUVCube);
        glDeleteProgram(g_program);
        glDeleteTextures(1, &g_texture);
    }
}
