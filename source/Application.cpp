#include "Application.h"
#include "Common.h"
#include "Shader.h"
#include "Texture.h"
#include "Camera.h"
#include "ObjLoader.h"
#include "VboIndexer.h"
#include <string>
#include "OpenGL.h"
#include <cmath>
#include <optional>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <inttypes.h>

//#define TEST_OBJ_FILE "suzanne.obj"
//#define TEST_BMP_FILE "suzanne.bmp"
#define TEST_OBJ_FILE "suzanne.obj"
#define TEST_BMP_FILE "suzanne.bmp"

namespace Application
{
    GLuint g_program;

    GLuint g_locationPosition;
    GLuint g_locationNormal;
    GLuint g_locationUV;
    GLuint g_locationMVP;
    GLuint g_locationM;
    GLuint g_locationV;
    GLuint g_locationTexture;
    GLuint g_locationLightPosition;

    GLuint g_bufferVertex;
    GLuint g_bufferNormal;
    GLuint g_bufferUV;
    GLuint g_bufferIndex;

    glm::mat4 g_MVP;
    glm::mat4 g_M;
    glm::mat4 g_V;
    glm::vec3 g_lightPositionWorldSpace;

    GLuint g_verticesCount;
    GLuint g_texture;
    CameraRotate g_camera;

    void InitLocations()
    {
        g_locationPosition = glGetAttribLocation(g_program, "positionModelSpace");
        g_locationNormal = glGetAttribLocation(g_program, "normalModelSpace");
        g_locationUV = glGetAttribLocation(g_program, "vertexUV");
        g_locationMVP = glGetUniformLocation(g_program, "MVP");
        g_locationM = glGetUniformLocation(g_program, "M");
        g_locationV = glGetUniformLocation(g_program, "V");
        g_locationTexture = glGetUniformLocation(g_program, "textureValue");
        g_locationLightPosition = glGetUniformLocation(g_program, "lightPositionWorldSpace");
    }

    // return number of vertices initialized
    uint32_t InitBuffers()
    {
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec2> uvs;
        std::vector<glm::vec3> normals;

        if (!LoadObj(TEST_OBJ_FILE, vertices, uvs, normals))
        {
            printf("Error loading obj file.");
            return 0;
        }

        auto indexed = VboIndex(vertices, uvs, normals);

        glGenBuffers(1, &g_bufferVertex);
        glBindBuffer(GL_ARRAY_BUFFER, g_bufferVertex);
        glBufferData(GL_ARRAY_BUFFER, indexed.vertices.size() * sizeof(glm::vec3), &indexed.vertices[0], GL_STATIC_DRAW);

        glGenBuffers(1, &g_bufferUV);
        glBindBuffer(GL_ARRAY_BUFFER, g_bufferUV);
        glBufferData(GL_ARRAY_BUFFER, indexed.uvs.size() * sizeof(glm::vec2), &indexed.uvs[0], GL_STATIC_DRAW);

        glGenBuffers(1, &g_bufferNormal);
        glBindBuffer(GL_ARRAY_BUFFER, g_bufferNormal);
        glBufferData(GL_ARRAY_BUFFER, indexed.normals.size() * sizeof(glm::vec3), &indexed.normals[0], GL_STATIC_DRAW);

        glGenBuffers(1, &g_bufferIndex);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_bufferIndex);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexed.indices.size() * sizeof(uint16_t), &indexed.indices[0], GL_STATIC_DRAW);

        return indexed.indices.size();
    }

    void DrawModel()
    {
        glUniformMatrix4fv(g_locationMVP, 1, GL_FALSE, &g_MVP[0][0]);
        glUniformMatrix4fv(g_locationM, 1, GL_FALSE, &g_M[0][0]);
        glUniformMatrix4fv(g_locationV, 1, GL_FALSE, &g_V[0][0]);

        glm::vec3 lightPosition = glm::vec3(4, 4, 4);
        glUniform3f(g_locationLightPosition, lightPosition.x, lightPosition.y, lightPosition.z);

        // Bind our texture in Texture Unit 0
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, g_texture);
        // Set our "textureValue" sampler to user Texture Unit 0
        glUniform1i(g_locationTexture, 0);

        glEnableVertexAttribArray(g_locationPosition);
        glBindBuffer(GL_ARRAY_BUFFER, g_bufferVertex);
        glVertexAttribPointer(g_locationPosition, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        glEnableVertexAttribArray(g_locationNormal);
        glBindBuffer(GL_ARRAY_BUFFER, g_bufferNormal);
        glVertexAttribPointer(g_locationNormal, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        glEnableVertexAttribArray(g_locationUV);
        glBindBuffer(GL_ARRAY_BUFFER, g_bufferUV);
        glVertexAttribPointer(g_locationUV, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_bufferIndex);

        glDrawElements(GL_TRIANGLES,      // mode
                       g_verticesCount,   // count
                       GL_UNSIGNED_SHORT, // type
                       (void*)0);         // element array buffer offset
 
        glDisableVertexAttribArray(g_locationPosition);
        glDisableVertexAttribArray(g_locationNormal);
        glDisableVertexAttribArray(g_locationUV);
    }

    void RecomputeMVPMatrix()
    {
        //g_camera.RecomputeMatrices();

        glm::mat4 projection = g_camera.GetProjectionMatrix();
        g_V = g_camera.GetViewMatrix();
        g_M = glm::mat4(1.0f);
        // Our ModelViewProjection : multiplication of our 3 matrices
        g_MVP = projection * g_V * g_M; // Remember, matrix multiplication is the other way around
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
        auto texture = LoadBMP(TEST_BMP_FILE);
        //auto texture = LoadDDS("suzanne.dds");
        if (!texture)
        {
            printf("Error loading texture.\n");
            return false;
        }
        g_texture = *texture;

        g_camera.Init();

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

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(g_program);

        DrawModel();

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
        glDeleteBuffers(1, &g_bufferVertex);
        glDeleteBuffers(1, &g_bufferNormal);
        glDeleteBuffers(1, &g_bufferUV);
        glDeleteBuffers(1, &g_bufferIndex);
        glDeleteProgram(g_program);
        glDeleteTextures(1, &g_texture);
    }
}
