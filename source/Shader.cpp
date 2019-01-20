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

void CheckGlError(const char * function)
{
#ifdef _DEBUG
    if (GLenum error = glGetError(); error != GL_NO_ERROR)
    {
        printf("OpenGl error calling function %s (%d)", function, error);
        throw std::runtime_error("OpenGl error");
    }
#endif
}

Shader::Shader(const char * vertexFile, const char * fragmentFile)
{
    m_program = CreateProgram(vertexFile, fragmentFile);
}

Shader::~Shader()
{
    if (m_program)
        glDeleteProgram(*m_program);
}

Shader::operator bool()
{
    return (bool)m_program;
}

template<class T>
void Shader::BindBuffer(GLuint buffer, const char * locationName, uint32_t offset, uint32_t stride)
{
    printf("Unsupported type to Shader::BindBuffer (location: %s)", locationName);
    throw std::runtime_error("Unsupported type to Shader::BindBuffer");
}

template<class T>
void Shader::BindBuffer(GLuint buffer, GLuint location, uint32_t offset, uint32_t stride)
{
    printf("Unsupported type to Shader::BindBuffer (location: %d)", location);
    throw std::runtime_error("Unsupported type to Shader::BindBuffer");
}

template<>
void Shader::BindBuffer<glm::vec3>(GLuint buffer, GLuint location, uint32_t offset, uint32_t stride)
{
    glEnableVertexAttribArray(location);
    CheckGlError("glEnableVertexAttribArray");

    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    CheckGlError("glBindBuffer");

    glVertexAttribPointer(location, 3, GL_FLOAT, GL_FALSE, stride, (void*)offset);
    CheckGlError("glVertexAttribPointer");

    m_currentLocations.push_back(location);
}

template<>
void Shader::BindBuffer<glm::vec3>(GLuint buffer, const char * locationName, uint32_t offset, uint32_t stride)
{
    GLuint location = GetLocation(locationName, LocationType::Attrib);

    BindBuffer<glm::vec3>(buffer, location, offset, stride);
}

template<>
void Shader::BindBuffer<glm::vec2>(GLuint buffer, GLuint location, uint32_t offset, uint32_t stride)
{
    glEnableVertexAttribArray(location);
    CheckGlError("glEnableVertexAttribArray");

    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    CheckGlError("glBindBuffer");

    glVertexAttribPointer(location, 2, GL_FLOAT, GL_FALSE, stride, (void*)offset);
    CheckGlError("glVertexAttribPointer");

    m_currentLocations.push_back(location);
}

template<>
void Shader::BindBuffer<glm::vec2>(GLuint buffer, const char * locationName, uint32_t offset, uint32_t stride)
{
    GLuint location = GetLocation(locationName, LocationType::Attrib);

    BindBuffer<glm::vec2>(buffer, location, offset, stride);
}

void Shader::BindElementBuffer(GLuint buffer)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
    CheckGlError("glBindBuffer");
}

template<class T>
void Shader::SetUniform(const T & value, const char * locationName)
{
    printf("Unsupported type to Shader::SetUniform (location: %s)", locationName);
    throw std::runtime_error("Unsupported type to Shader::SetUniform");
}

template<class T>
void Shader::SetUniform(const T & value, GLuint location)
{
    printf("Unsupported type to Shader::SetUniform (location: %d)", location);
    throw std::runtime_error("Unsupported type to Shader::SetUniform");
}

template<>
void Shader::SetUniform<glm::mat4>(const glm::mat4 & value, GLuint location)
{
    glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
    CheckGlError("glUniformMatrix4fv");
}

template<>
void Shader::SetUniform<glm::mat4>(const glm::mat4 & value, const char * locationName)
{
    GLuint location = GetLocation(locationName, LocationType::Uniform);
    SetUniform<glm::mat4>(value, location);
}

template<>
void Shader::SetUniform<glm::mat3>(const glm::mat3 & value, GLuint location)
{
    glUniformMatrix3fv(location, 1, GL_FALSE, &value[0][0]);
    CheckGlError("glUniformMatrix3fv");
}

template<>
void Shader::SetUniform<glm::mat3>(const glm::mat3 & value, const char * locationName)
{
    GLuint location = GetLocation(locationName, LocationType::Uniform);
    SetUniform<glm::mat3>(value, location);
}

template<>
void Shader::SetUniform<glm::vec3>(const glm::vec3 & value, GLuint location)
{
    glUniform3f(location, value.x, value.y, value.z);
    CheckGlError("glUniform3f");
}

template<>
void Shader::SetUniform<glm::vec3>(const glm::vec3 & value, const char * locationName)
{
    GLuint location = GetLocation(locationName, LocationType::Uniform);
    SetUniform<glm::vec3>(value, location);
}

void Shader::BindTexture(GLuint texture, GLuint location)
{
    glActiveTexture(GL_TEXTURE0 + m_currentTexture);
    CheckGlError("glActiveTexture");

    glBindTexture(GL_TEXTURE_2D, texture);
    CheckGlError("glBindTexture");

    glUniform1i(location, m_currentTexture);
    CheckGlError("glUniform1i");

    m_currentTexture++;
}

void Shader::BindTexture(GLuint texture, const char * locationName)
{
    GLuint location = GetLocation(locationName, LocationType::Uniform);
    BindTexture(texture, location);
}

void Shader::Begin()
{
    glUseProgram(*m_program);
    CheckGlError("glUseProgram");
}

void Shader::End()
{
    for (const auto & v : m_currentLocations)
        glDisableVertexAttribArray(v);

    m_currentTexture = 0;
}

GLuint Shader::GetLocation(const char * locationName, LocationType type)
{
    auto it = m_locations.find(locationName);
    if (it != std::end(m_locations))
        return it->second;

    GLuint newLocation = -1;
    if (type == LocationType::Attrib)
        newLocation = glGetAttribLocation(*m_program, locationName);
    else
        newLocation = glGetUniformLocation(*m_program, locationName);

    if (newLocation == -1)
    {
        printf("Requested invalid location in shader: %s", locationName);
        throw std::runtime_error("Requested invalid location in shader.");
    }

    m_locations[locationName] = newLocation;

    return newLocation;
}

std::vector<GLuint> Shader::GetLocations(const std::vector<std::tuple<std::string, LocationType>> & locations)
{
    std::vector<GLuint> result;
    for (const auto &[name, type] : locations)
        result.push_back(GetLocation(name.c_str(), type));
    return result;
}

//template void Shader::BindBuffer<glm::vec3>(GLuint buffer, const char * location, uint32_t offset, uint32_t stride);
//template void Shader::BindBuffer<glm::vec2>(GLuint buffer, const char * location, uint32_t offset, uint32_t stride);
//
//template void Shader::SetUniform<glm::mat4>(const glm::mat4 & value, const char * location);
//template void Shader::SetUniform<glm::mat3>(const glm::mat3 & value, const char * location);
//template void Shader::SetUniform<glm::vec3>(const glm::vec3 & value, const char * location);
