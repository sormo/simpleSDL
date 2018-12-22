#include "Application.h"
#include "Common.h"
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

    static const GLfloat g_dataPositionTriangle[] =
    {
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        0.0f,  1.0f, 0.0f
    };

    static const GLfloat g_dataColorTriangle[] =
    {
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
    GLuint g_bufferVertexTriangle;
    GLuint g_bufferColorTriangle;

    glm::mat4 g_MVPCube;
    glm::mat4 g_MVPTriangle;

    std::optional<GLuint> CompileShader(const char * fileName, GLenum type)
    {
        auto data = Common::ReadFile(fileName);
        data.push_back('\0');
        const GLchar * str = (const GLchar*)data.data();

        GLuint result = glCreateShader(type);

        glShaderSource(result, 1, &str, NULL);
        glCompileShader(result);

        //Check vertex shader for errors
        GLint shaderCompiled = GL_FALSE;
        glGetShaderiv(result, GL_COMPILE_STATUS, &shaderCompiled);
        if (shaderCompiled != GL_TRUE)
        {
            GLint logLength;
            glGetShaderiv(result, GL_INFO_LOG_LENGTH, &logLength);
            if (logLength > 0)
            {
                std::vector<GLchar> buffer(logLength);
                glGetShaderInfoLog(result, logLength, &logLength, buffer.data());
                
                printf("Shader compile log: %s\n", buffer.data());
            }

            glDeleteShader(result);

            return std::nullopt;
        }

        return result;
    }

    void PrintProgramInfo(GLuint program)
    {
        GLint logLen;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLen);
        if (logLen > 0)
        {
            std::vector<GLchar> buffer(logLen);
            // Show any errors as appropriate
            glGetProgramInfoLog(program, logLen, &logLen, buffer.data());

            printf("Program Info Log:\n%s\n", buffer.data());
        }
    }

    std::optional<GLuint> CreateProgram()
    {
        auto vertexShader = CompileShader("shaderVertex.glsl", GL_VERTEX_SHADER);
        auto fragmentShader = CompileShader("shaderFragment.glsl", GL_FRAGMENT_SHADER);

        if (!vertexShader || !fragmentShader)
            return std::nullopt;

        // Create program, attach shaders to it, and link it
        GLuint program = glCreateProgram();

        glAttachShader(program, *vertexShader);
        glAttachShader(program, *fragmentShader);

        //// must be done before linking
        //glBindAttribLocation(program, VERTEX_INDEX, "position");
        //glBindAttribLocation(program, COLOR_INDEX, "color");

        glLinkProgram(program);

        GLint linked;
        glGetProgramiv(program, GL_LINK_STATUS, &linked);
        if (!linked)
        {
            PrintProgramInfo(program);

            return std::nullopt;
        }

#ifdef _DEBUG
        glValidateProgram(program);
        GLint valid;
        glGetProgramiv(program, GL_VALIDATE_STATUS, &valid);
        if (!valid)
        {
            PrintProgramInfo(program);

            return std::nullopt;
        }
#endif

        glDetachShader(program, *vertexShader);
        glDetachShader(program, *fragmentShader);

        // Delete the shaders as the program has them now
        glDeleteShader(*vertexShader);
        glDeleteShader(*fragmentShader);

        return program;
    }

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

        glGenBuffers(1, &g_bufferVertexTriangle);
        glBindBuffer(GL_ARRAY_BUFFER, g_bufferVertexTriangle);
        glBufferData(GL_ARRAY_BUFFER, sizeof(g_dataPositionTriangle), g_dataPositionTriangle, GL_STATIC_DRAW);

        glGenBuffers(1, &g_bufferColorTriangle);
        glBindBuffer(GL_ARRAY_BUFFER, g_bufferColorTriangle);
        glBufferData(GL_ARRAY_BUFFER, sizeof(g_dataColorTriangle), g_dataColorTriangle, GL_STATIC_DRAW);
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

    void DrawTriangle()
    {
        glUniformMatrix4fv(g_locationMVP, 1, GL_FALSE, &g_MVPTriangle[0][0]);

        glEnableVertexAttribArray(g_locationPosition);
        glBindBuffer(GL_ARRAY_BUFFER, g_bufferVertexTriangle);
        glVertexAttribPointer(g_locationPosition, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        glEnableVertexAttribArray(g_locationColor);
        glBindBuffer(GL_ARRAY_BUFFER, g_bufferColorTriangle);
        glVertexAttribPointer(g_locationColor, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        glDrawArrays(GL_TRIANGLES, 0, 3); // single triangle

        glDisableVertexAttribArray(g_locationPosition);
        glDisableVertexAttribArray(g_locationColor);
    }

    void InitMatrices()
    {
        // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
        glm::mat4 projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
        // Camera matrix
        glm::mat4 viewCube = glm::lookAt(
            glm::vec3(3, 4, 3), // Camera is at (4,3,-3), in World Space
            glm::vec3(0, 0, 0), // and looks at the origin
            glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
        );
        // Model matrix : an identity matrix (model will be at the origin)
        glm::mat4 modelCube = glm::mat4(1.0f);
        // Our ModelViewProjection : multiplication of our 3 matrices
        g_MVPCube = projection * viewCube * modelCube; // Remember, matrix multiplication is the other way around

        // identity matrix for triangle
        glm::mat4 viewTriangle = glm::lookAt(glm::vec3{ 0,0,4 }, glm::vec3{ 0,0,0 }, glm::vec3{ 0,1,0 });
        glm::mat4 modelTriangle = glm::translate(glm::mat4(1.0), { 1.5, 1.0, 0.0 });
        g_MVPTriangle = projection * viewTriangle * modelTriangle;
    }

    bool Init()
    {
        if (!InitOpenGL())
        {
            printf("Error initializing OpenGl\n");
            PrintOpenGlPointers();
            return false;
        }

        auto program = CreateProgram();
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

    void ReinitTriangleMatrix(double seconds)
    {
        glm::mat4 projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
        glm::mat4 view = glm::lookAt(glm::vec3{ 0,0,4 }, glm::vec3{ 0,0,0 }, glm::vec3{ 0,1,0 });
        glm::mat4 model = glm::mat4(1.0); 
    
        static float rotate = 0.0f;
        rotate += 0.01f;
        model = glm::translate(glm::mat4(1.0), { 1.5, 1.0, 0.0 });
        model = glm::rotate(model, rotate, { 0.0, 1.0, 0.0 });
        
        g_MVPTriangle = projection * view * model;
    }

    void ReloadTriangleColors(double seconds)
    {
        const GLfloat color[] = {(float)std::sin(seconds) * 0.5f + 0.5f,
                                 (float)std::cos(seconds) * 0.5f + 0.5f,
                                  0.0f};

        GLfloat colorData[9] = { color[0], color[1], color[2],
                                 color[0], color[1], color[2],
                                 color[0], color[1], color[2] };

        glBindBuffer(GL_ARRAY_BUFFER, g_bufferColorTriangle);
        glBufferData(GL_ARRAY_BUFFER, sizeof(colorData), colorData, GL_STATIC_DRAW);
    }

    bool MainLoop()
    {
        double seconds = GetCurrentTime();

        ReinitTriangleMatrix(seconds);
        ReloadTriangleColors(seconds);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(g_program);

        DrawCube();
        DrawTriangle();

        return true;
    }

    void Deinit()
    {
        glDeleteBuffers(1, &g_bufferVertexCube);
        glDeleteBuffers(1, &g_bufferColorCube);
        glDeleteBuffers(1, &g_bufferVertexTriangle);
        glDeleteBuffers(1, &g_bufferColorTriangle);
        glDeleteProgram(g_program);
    }
}
