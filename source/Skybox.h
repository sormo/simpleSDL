#pragma once
#include <vector>
#include <string>
#include "Texture.h"
#include "Shader.h"


class Skybox
{
public:
    // textures in following order: right, left, top, bottom, back, front
    Skybox(const std::vector<std::string> & paths);
    ~Skybox();

    void Draw(const glm::mat4 & view, const glm::mat4 & projection);

private:
    Shader m_shader;

    GLuint m_texture;
    GLuint m_vao;
    GLuint m_vbo;

    // TODO rename
    GLuint m_positionAttributeLocation;
    GLuint m_projectionUniformLocation;
    GLuint m_viewUnifomtmLocation;
    GLuint m_samplerUniformLocation;
};
