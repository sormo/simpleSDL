#include "Skybox.h"
#include "Common.h"

static const float SKYBOX_VERTICES[] =
{
    // positions
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
};

// textures in following order: right, left, top, bottom, back, front

Skybox::Skybox(const std::vector<std::string>& paths)
    : m_shader(Common::ReadFileToString("shaders/vertSkybox.glsl").c_str(), Common::ReadFileToString("shaders/fragSkybox.glsl").c_str())
{
    if (!m_shader)
        throw std::runtime_error("Error initializing skybox shader.");

    m_positionAttributeLocation = m_shader.GetLocation("position", Shader::LocationType::Attrib);
    m_projectionUniformLocation = m_shader.GetLocation("projection", Shader::LocationType::Uniform);
    m_viewUnifomtmLocation = m_shader.GetLocation("view", Shader::LocationType::Uniform);
    m_samplerUniformLocation = m_shader.GetLocation("skybox", Shader::LocationType::Uniform);

    auto texture = Texture::LoadCubemap(paths);
    if (!texture)
        throw std::runtime_error("Error loading cubemap textures.");

    m_texture = *texture;

    if (IsVAOSupported())
    {
        glGenVertexArrays(1, &m_vao);
        glBindVertexArray(m_vao);
    }

    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    
    glBufferData(GL_ARRAY_BUFFER, sizeof(SKYBOX_VERTICES), &SKYBOX_VERTICES, GL_STATIC_DRAW);

    if (IsVAOSupported())
    {
        glEnableVertexAttribArray(m_positionAttributeLocation);
        glVertexAttribPointer(m_positionAttributeLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

        glBindVertexArray(0);
    }
}

Skybox::~Skybox()
{
    glDeleteTextures(1, &m_texture);
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vbo);
}

void Skybox::Draw(const glm::mat4 & view, const glm::mat4 & projection)
{
    glm::mat4 viewSky = glm::mat4(glm::mat3(view)); // remove translation from the view matrix

    glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
    
    m_shader.BeginRender();

    m_shader.SetUniform(viewSky, m_viewUnifomtmLocation);
    m_shader.SetUniform(projection, m_projectionUniformLocation);

    if (IsVAOSupported())
    {
        m_shader.BindVAO(m_vao);
    }
    else
    {
        m_shader.BindBuffer<glm::vec3>(m_vbo, m_positionAttributeLocation);
    }

    m_shader.BindCubemapTexture(m_texture, m_samplerUniformLocation);

    glDrawArrays(GL_TRIANGLES, 0, 36);
    CheckGlError("glDrawArrays");

    m_shader.EndRender();

    glDepthFunc(GL_LESS); // set depth function back to default
}

GLuint Skybox::GetSkyboxTexture()
{
    return m_texture;
}
