#pragma once
#include "OpenGL.h"
#include <optional>
#include <string>
#include <vector>
#include <unordered_map>
#include "glm/glm.hpp"

std::optional<GLuint> CreateProgram(const char * vertexFile, const char * fragmentFile);

class Shader
{
public:
    Shader(const char * vertexFile, const char * fragmentFile);
    ~Shader();

    enum class LocationType { Attrib, Uniform };
    std::vector<GLuint> GetLocations(const std::vector<std::tuple<std::string, LocationType>> & locations);

    template<class T>
    void BindBuffer(GLuint buffer, const char * locationName, uint32_t offset = 0, uint32_t stride = 0);
    template<class T>
    void BindBuffer(GLuint buffer, GLuint location, uint32_t offset = 0, uint32_t stride = 0);

    void BindElementBuffer(GLuint buffer);

    template<class T>
    void SetUniform(const T & value, const char * locationName);
    template<class T>
    void SetUniform(const T & value, GLuint location);

    void BindTexture(GLuint texture, const char * locationName);
    void BindTexture(GLuint texture, GLuint location);

    void Begin();
    void End();

    operator bool();

private:

    GLuint GetLocation(const char * locationName, LocationType type);

    std::unordered_map<std::string, GLuint> m_locations;
    std::optional<GLuint> m_program;

    GLuint m_currentTexture = 0;
    std::vector<GLuint> m_currentLocations;
};

//extern template void Shader::BindBuffer<glm::vec3>(GLuint buffer, const char * location, uint32_t offset, uint32_t stride);
//extern template void Shader::BindBuffer<glm::vec2>(GLuint buffer, const char * location, uint32_t offset, uint32_t stride);
//
//extern template void Shader::SetUniform<glm::mat4>(const glm::mat4 & value, const char * location);
//extern template void Shader::SetUniform<glm::mat3>(const glm::mat3 & value, const char * location);
//extern template void Shader::SetUniform<glm::vec3>(const glm::vec3 & value, const char * location);
