#include "Shader.h"
#include "Common.h"
#include <vector>

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

std::optional<GLuint> CreateProgram(const char * vertexFile, const char * fragmentFile)
{
    auto vertexShader = CompileShader(vertexFile, GL_VERTEX_SHADER);
    auto fragmentShader = CompileShader(fragmentFile, GL_FRAGMENT_SHADER);

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
