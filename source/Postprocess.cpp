#include "Postprocess.h"
#include "OpenGL.h"
#include "Common.h"

// vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
static const float SCREEN_VERTICES[] =
{
    // positions   // texCoords
    -1.0f,  1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
     1.0f, -1.0f,  1.0f, 0.0f,

    -1.0f,  1.0f,  0.0f, 1.0f,
     1.0f, -1.0f,  1.0f, 0.0f,
     1.0f,  1.0f,  1.0f, 1.0f
};

const char * GetShaderForType(Postprocess::Type type)
{
    switch (type)
    {
    case Postprocess::Type::InvertColors:
        return "shaders/fragPostInvert.glsl";
    case Postprocess::Type::NoChange:
        return "shaders/fragPostNoChange.glsl";
    case Postprocess::Type::Grayscale:
        return "shaders/fragPostGrayscale.glsl";
    case Postprocess::Type::KernelBlur:
    case Postprocess::Type::KernelEdge:
    case Postprocess::Type::KernelSharpen:
        return "shaders/fragPostKernel.glsl";
    }
}

std::vector<float> GetKernelForType(Postprocess::Type type)
{
    switch (type)
    {
    case Postprocess::Type::KernelBlur:
        return { 1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f,
                 2.0f / 16.0f, 4.0f / 16.0f, 2.0f / 16.0f,
                 1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f };
    case Postprocess::Type::KernelEdge:
        return { 1.0f,  1.0f, 1.0f,
                 1.0f, -8.0f, 1.0f,
                 1.0f,  1.0f, 1.0f };
    case Postprocess::Type::KernelSharpen:
        return { 2.0f,   2.0f, 2.0f,
                 2.0f, -15.0f, 2.0f,
                 2.0f,   2.0f, 2.0f };
    }
    return {};
}

Postprocess::Postprocess(Type type)
    : m_shader(Common::ReadFileToString("shaders/vertPostprocess.glsl").c_str(), Common::ReadFileToString(GetShaderForType(type)).c_str())
{
    m_kernel = GetKernelForType(type);

    if (!m_shader)
        throw std::runtime_error("Error creating postprocess shader.");

    InitLocations();
    InitBuffers();
}

void Postprocess::InitLocations()
{
    m_locationPosition = m_shader.GetLocation("position", Shader::LocationType::Attrib);
    m_locationVertexUV = m_shader.GetLocation("vertexUV", Shader::LocationType::Attrib);
    m_locationTexture = m_shader.GetLocation("screenTexture", Shader::LocationType::Uniform);
    if (!m_kernel.empty())
        m_locationKernel = m_shader.GetLocation("kernel", Shader::LocationType::Uniform);
}

void Postprocess::InitBuffers()
{
    if (IsVAOSupported())
    {
        glGenVertexArrays(1, &m_vao);
        glBindVertexArray(m_vao);
    }

    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

    glBufferData(GL_ARRAY_BUFFER, sizeof(SCREEN_VERTICES), &SCREEN_VERTICES, GL_STATIC_DRAW);

    if (IsVAOSupported())
    {
        glEnableVertexAttribArray(m_locationPosition);
        glVertexAttribPointer(m_locationPosition, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

        glEnableVertexAttribArray(m_locationVertexUV);
        glVertexAttribPointer(m_locationVertexUV, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

        glBindVertexArray(0);
    }
}

void Postprocess::Draw()
{
    m_shader.BeginRender();

    m_shader.BindTexture(m_framebuffer.GetTextureAttachment(), m_locationTexture);

    // TODO set uniform throught shader ??? possible not worth it ???
    if (!m_kernel.empty())
        glUniform1fv(m_locationKernel, 9, m_kernel.data());

    if (IsVAOSupported())
    {
        m_shader.BindVAO(m_vao);
    }
    else
    {
        m_shader.BindBuffer<glm::vec2>(m_vbo, m_locationPosition, 0, 4 * sizeof(float));
        m_shader.BindBuffer<glm::vec2>(m_vbo, m_locationVertexUV, 2 * sizeof(float), 4 * sizeof(float));
    }

    glDrawArrays(GL_TRIANGLES, 0, 6);
    CheckGlError("glDrawArrays");

    m_shader.EndRender();
}

void Postprocess::BeginRender()
{
    m_framebuffer.BeginRender();
}

void Postprocess::EndRender()
{
    m_framebuffer.EndRender();

    glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // draw to default framebuffer
    Draw();

    glEnable(GL_DEPTH_TEST);
}
