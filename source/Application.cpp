#include "Application.h"
#include "Common.h"
#include "Shader.h"
#include <string>
#include "OpenGL.h"
#include <chrono>
#include <cmath>
#include <optional>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace Application
{
    static const GLfloat g_dataPositionCube[] =
    {
        -1.0f,-1.0f,-1.0f, // triangle 1 : begin
        -1.0f,-1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f, // triangle 1 : end
        1.0f, 1.0f,-1.0f, // triangle 2 : begin
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f,-1.0f, // triangle 2 : end
        1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f,-1.0f,
        1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,
        1.0f,-1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f,-1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f,-1.0f,
        -1.0f, 1.0f,-1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f,-1.0f, 1.0f
    };

    static const GLfloat g_dataColorCube[] =
    {
        0.583f,  0.771f,  0.014f,
        0.609f,  0.115f,  0.436f,
        0.327f,  0.483f,  0.844f,
        0.822f,  0.569f,  0.201f,
        0.435f,  0.602f,  0.223f,
        0.310f,  0.747f,  0.185f,
        0.597f,  0.770f,  0.761f,
        0.559f,  0.436f,  0.730f,
        0.359f,  0.583f,  0.152f,
        0.483f,  0.596f,  0.789f,
        0.559f,  0.861f,  0.639f,
        0.195f,  0.548f,  0.859f,
        0.014f,  0.184f,  0.576f,
        0.771f,  0.328f,  0.970f,
        0.406f,  0.615f,  0.116f,
        0.676f,  0.977f,  0.133f,
        0.971f,  0.572f,  0.833f,
        0.140f,  0.616f,  0.489f,
        0.997f,  0.513f,  0.064f,
        0.945f,  0.719f,  0.592f,
        0.543f,  0.021f,  0.978f,
        0.279f,  0.317f,  0.505f,
        0.167f,  0.620f,  0.077f,
        0.347f,  0.857f,  0.137f,
        0.055f,  0.953f,  0.042f,
        0.714f,  0.505f,  0.345f,
        0.783f,  0.290f,  0.734f,
        0.722f,  0.645f,  0.174f,
        0.302f,  0.455f,  0.848f,
        0.225f,  0.587f,  0.040f,
        0.517f,  0.713f,  0.338f,
        0.053f,  0.959f,  0.120f,
        0.393f,  0.621f,  0.362f,
        0.673f,  0.211f,  0.457f,
        0.820f,  0.883f,  0.371f,
        0.982f,  0.099f,  0.879f
    };

    GLuint g_program;

    GLuint g_locationPosition;
    GLuint g_locationColor;
    GLuint g_locationMVP;

    GLuint g_bufferVertexCube;
    GLuint g_bufferColorCube;

    glm::mat4 g_MVPCube;

    void InitLocations()
    {
        g_locationPosition = glGetAttribLocation(g_program, "position");
        g_locationColor = glGetAttribLocation(g_program, "color");
        g_locationMVP = glGetUniformLocation(g_program, "MVP");
    }

    void InitBuffers()
    {
        glGenBuffers(1, &g_bufferVertexCube);
        glBindBuffer(GL_ARRAY_BUFFER, g_bufferVertexCube);
        glBufferData(GL_ARRAY_BUFFER, sizeof(g_dataPositionCube), g_dataPositionCube, GL_STATIC_DRAW);

        glGenBuffers(1, &g_bufferColorCube);
        glBindBuffer(GL_ARRAY_BUFFER, g_bufferColorCube);
        glBufferData(GL_ARRAY_BUFFER, sizeof(g_dataColorCube), g_dataColorCube, GL_STATIC_DRAW);
    }

    void DrawCube()
    {
        glUniformMatrix4fv(g_locationMVP, 1, GL_FALSE, &g_MVPCube[0][0]);

        glEnableVertexAttribArray(g_locationPosition);
        glBindBuffer(GL_ARRAY_BUFFER, g_bufferVertexCube);
        glVertexAttribPointer(g_locationPosition, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        glEnableVertexAttribArray(g_locationColor);
        glBindBuffer(GL_ARRAY_BUFFER, g_bufferColorCube);
        glVertexAttribPointer(g_locationColor, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        glDrawArrays(GL_TRIANGLES, 0, 12 * 3); // 12 triangles, two for each of six sides
 
        glDisableVertexAttribArray(g_locationPosition);
        glDisableVertexAttribArray(g_locationColor);
    }

    void InitMatrices()
    {
        auto[width, height] = Common::GetWindowSize();

        // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
        glm::mat4 projection = glm::perspective(45.0f, (float)width / (float)height, 0.1f, 100.0f);
        // Camera matrix
        glm::mat4 viewCube = glm::lookAt(
            glm::vec3(3, 4, -3), // Camera is at (4,3,-3), in World Space
            glm::vec3(0, 0, 0), // and looks at the origin
            glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
        );
        // Model matrix : an identity matrix (model will be at the origin)
        glm::mat4 modelCube = glm::mat4(1.0f);
        // Our ModelViewProjection : multiplication of our 3 matrices
        g_MVPCube = projection * viewCube * modelCube; // Remember, matrix multiplication is the other way around
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
        InitBuffers();

        InitMatrices();

        return true;
    }

    double GetCurrentTime()
    {
        double currentTime = (double)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        return (double)currentTime / 1000.0f;
    }

    bool MainLoop()
    {
        double seconds = GetCurrentTime();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(g_program);

        DrawCube();

        return true;
    }

    void Deinit()
    {
        glDeleteBuffers(1, &g_bufferVertexCube);
        glDeleteBuffers(1, &g_bufferColorCube);
        glDeleteProgram(g_program);
    }
}
