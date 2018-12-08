#include "Application.h"
#include "Common.h"
#include <string>
#include "OpenGL.h"
#include <chrono>
#include <cmath>
#include <optional>

namespace Application
{
    GLuint g_program;
    GLuint g_buffer;
    GLuint g_vertexLocation;

    static const GLfloat VERTICES[] = { 0.25f, -0.25f, 0.5f,
                                       -0.25f, -0.25f, 0.5f,
                                        0.25f,  0.25f, 0.5f };


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

        glGenBuffers(1, &g_buffer);

        // Load the vertex data
        glBindBuffer(GL_ARRAY_BUFFER, g_buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(VERTICES), VERTICES, GL_STATIC_DRAW);

        g_vertexLocation = 0;
        glBindAttribLocation(program, g_vertexLocation, "position");
        //g_vertexLocation = glGetAttribLocation(program, "position");

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

        // Delete the shaders as the program has them now
        glDeleteShader(*vertexShader);
        glDeleteShader(*fragmentShader);

        return program;
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

        const GLfloat color[] = {(float)std::sin(seconds) * 0.5f + 0.5f,
                                 (float)std::cos(seconds) * 0.5f + 0.5f,
                                  0.0f, 1.0f };
        glClearColor(color[0], color[1], color[2], color[3]);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(g_program);

        glEnableVertexAttribArray(g_vertexLocation);
        glBindBuffer(GL_ARRAY_BUFFER, g_buffer);
        glVertexAttribPointer(g_vertexLocation, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        glDisableVertexAttribArray(g_vertexLocation);

        return true;
    }

    void Deinit()
    {
        glDeleteBuffers(1, &g_buffer);
        glDeleteProgram(g_program);
    }
}
