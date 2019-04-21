#include "Shader.h"
#include "Common.h"
#include <vector>

std::optional<GLuint> CompileShader(const char * data, GLenum type)
{
    GLuint result = glCreateShader(type);

    glShaderSource(result, 1, &data, NULL);
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

std::optional<GLuint> CreateAndLinkProgram(const char * vertexData, const char * geometryData, const char * fragmentData, std::function<void(GLuint)> bindCallback)
{
    std::optional<GLuint> vertexShader = CompileShader(vertexData, GL_VERTEX_SHADER);
    std::optional<GLuint> geometryShader;
    std::optional<GLuint> fragmentShader = CompileShader(fragmentData, GL_FRAGMENT_SHADER);

    if (!vertexShader || !fragmentShader)
        return std::nullopt;

    if (geometryData)
        geometryShader = CompileShader(geometryData, GL_GEOMETRY_SHADER);

    if (geometryData && !geometryShader)
        return std::nullopt;

    // Create program, attach shaders to it, and link it
    GLuint program = glCreateProgram();

    glAttachShader(program, *vertexShader);
    glAttachShader(program, *fragmentShader);
    if (geometryShader)
        glAttachShader(program, *geometryShader);

    // bind locations before linking
    if (bindCallback)
        bindCallback(program);

    glLinkProgram(program);

    GLint linked;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (!linked)
    {
        PrintProgramInfo(program);

        return std::nullopt;
    }

    glDetachShader(program, *vertexShader);
    glDetachShader(program, *fragmentShader);

    // Delete the shaders as the program has them now
    glDeleteShader(*vertexShader);
    glDeleteShader(*fragmentShader);

    return program;
}

std::optional<GLuint> CreateAndLinkProgramFile(const char * vertexFile, const char * geometryFile, const char * fragmentFile, std::function<void(GLuint)> bindCallback)
{
    printf("Compiling shaders from files: %s %s", vertexFile, fragmentFile);

    auto vertexData = Common::ReadFile(vertexFile);
    vertexData.push_back('\0');

    auto fragmentData = Common::ReadFile(fragmentFile);
    fragmentData.push_back('\0');

    std::vector<uint8_t> geometryData;
    if (geometryFile)
    {
        geometryData = Common::ReadFile(geometryFile);
        geometryData.push_back('\0');
    }

    return CreateAndLinkProgram((const char *)vertexData.data(), geometryData.empty() ? nullptr : (const char *)geometryData.data(), (const char *)fragmentData.data(), bindCallback);
}

Shader::Shader(const char * vertex, const char * fragment)
{
    m_program = CreateAndLinkProgram(vertex, nullptr, fragment);
}

Shader::Shader(const char * vertex, const char * geometry, const char * fragment)
{
    m_program = CreateAndLinkProgram(vertex, geometry, fragment);
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

void Shader::BindVAO(GLuint vao)
{
    glBindVertexArray(vao);
    m_boundVAO = true;
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
void Shader::BindBuffer<glm::vec4>(GLuint buffer, GLuint location, uint32_t offset, uint32_t stride)
{
    glEnableVertexAttribArray(location);
    CheckGlError("glEnableVertexAttribArray");

    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    CheckGlError("glBindBuffer");

    glVertexAttribPointer(location, 4, GL_FLOAT, GL_FALSE, stride, (void*)offset);
    CheckGlError("glVertexAttribPointer");

    m_boundLocations.push_back(location);
}

template<>
void Shader::BindBuffer<glm::vec4>(GLuint buffer, const char* locationName, uint32_t offset, uint32_t stride)
{
    GLuint location = GetLocation(locationName, LocationType::Attrib);

    BindBuffer<glm::vec4>(buffer, location, offset, stride);
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

    m_boundLocations.push_back(location);
}

template<>
void Shader::BindBuffer<glm::vec3>(GLuint buffer, const char* locationName, uint32_t offset, uint32_t stride)
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

    m_boundLocations.push_back(location);
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

template<>
void Shader::SetUniform<glm::vec2>(const glm::vec2 & value, GLuint location)
{
    glUniform2f(location, value.x, value.y);
    CheckGlError("glUniform3f");
}

template<>
void Shader::SetUniform<glm::vec2>(const glm::vec2 & value, const char * locationName)
{
    GLuint location = GetLocation(locationName, LocationType::Uniform);
    SetUniform<glm::vec2>(value, location);
}

template<>
void Shader::SetUniform<float>(const float & value, GLuint location)
{
    glUniform1f(location, value);
    CheckGlError("glUniform1f");
}

template<>
void Shader::SetUniform<float>(const float & value, const char * locationName)
{
    GLuint location = GetLocation(locationName, LocationType::Uniform);
    SetUniform<float>(value, location);
}

template<>
void Shader::SetUniform<GLint>(const GLint & value, GLuint location)
{
    glUniform1i(location, value);
    CheckGlError("glUniform1i");
}

template<>
void Shader::SetUniform<GLint>(const GLint & value, const char * locationName)
{
    GLuint location = GetLocation(locationName, LocationType::Uniform);
    SetUniform<GLint>(value, location);
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

void Shader::BindCubemapTexture(GLuint texture, const char * locationName)
{
    GLuint location = GetLocation(locationName, LocationType::Uniform);
    BindCubemapTexture(texture, location);
}

void Shader::BindCubemapTexture(GLuint texture, GLuint location)
{
    glActiveTexture(GL_TEXTURE0 + m_currentTexture);
    CheckGlError("glActiveTexture");

    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
    CheckGlError("glBindTexture");

    glUniform1i(location, m_currentTexture);
    CheckGlError("glUniform1i");

    m_currentTexture++;
}

void Shader::BeginRender()
{
    glUseProgram(*m_program);
    CheckGlError("glUseProgram");
}

void Shader::EndRender()
{
#ifdef _DEBUG
    Validate();
#endif

    if (m_boundVAO)
    {
        glBindVertexArray(0);
    }
    else
    {
        for (auto location : m_boundLocations)
            glDisableVertexAttribArray(location);
        m_boundLocations.clear();
    }

    glActiveTexture(GL_TEXTURE0);
    m_currentTexture = 0;

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glUseProgram(0);
}

GLuint Shader::GetLocation(const std::string & locationName, LocationType type)
{
    return GetLocation(locationName.c_str(), type);
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

    CheckGlError("glGetUniformLocation");

    if (newLocation == -1)
    {
        printf("Requested invalid location in shader: %s\n", locationName);
        // TODO sometimes location can be optimized away according to constants of material
        //throw std::runtime_error("Requested invalid location in shader.");
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

void Shader::PrintUniforms()
{
    GLint count;
    glGetProgramiv(*m_program, GL_ACTIVE_UNIFORMS, &count);
    printf("Active Uniforms: %d\n", count);

    GLint size; // size of the variable
    GLenum type; // type of the variable (float, vec3 or mat4, etc)

    const GLsizei bufSize = 128; // maximum name length
    GLchar name[bufSize]; // variable name in GLSL
    GLsizei length; // name length

    for (GLint i = 0; i < count; i++)
    {
        glGetActiveUniform(*m_program, (GLuint)i, bufSize, &length, &size, &type, name);

        printf("Uniform #%d Type: %u Name: %s\n", i, type, name);
    }
}

void Shader::PrintAttributes()
{
    GLint count;
    glGetProgramiv(*m_program, GL_ACTIVE_ATTRIBUTES, &count);
    printf("Active Attributes: %d\n", count);

    GLint size; // size of the variable
    GLenum type; // type of the variable (float, vec3 or mat4, etc)

    const GLsizei bufSize = 128; // maximum name length
    GLchar name[bufSize]; // variable name in GLSL
    GLsizei length; // name length

    for (GLint i = 0; i < count; i++)
    {
        glGetActiveAttrib(*m_program, (GLuint)i, bufSize, &length, &size, &type, name);

        printf("Attribute #%d Type: %u Name: %s\n", i, type, name);
    }
}

void Shader::Validate()
{
    if (m_validated)
        return;
    m_validated = true;

    glValidateProgram(*m_program);
    GLint valid;
    glGetProgramiv(*m_program, GL_VALIDATE_STATUS, &valid);
    if (!valid)
    {
        PrintProgramInfo(*m_program);
        throw std::runtime_error("Shader not valid");
    }
}

//template void Shader::BindBuffer<glm::vec3>(GLuint buffer, const char * location, uint32_t offset, uint32_t stride);
//template void Shader::BindBuffer<glm::vec2>(GLuint buffer, const char * location, uint32_t offset, uint32_t stride);
//
//template void Shader::SetUniform<glm::mat4>(const glm::mat4 & value, const char * location);
//template void Shader::SetUniform<glm::mat3>(const glm::mat3 & value, const char * location);
//template void Shader::SetUniform<glm::vec3>(const glm::vec3 & value, const char * location);
