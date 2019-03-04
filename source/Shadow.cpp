#include "Shadow.h"
#include "Common.h"
#include "glm/gtc/matrix_transform.hpp"

static const GLsizei SHADOW_WIDTH = 1024;
static const GLsizei SHADOW_HEIGHT = 1024;

static const float NEAR_PLANE = 1.0f;
static const float FAR_PLANE = 7.5f;

glm::mat4 LIGHT_PROJECTION = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, NEAR_PLANE, FAR_PLANE);

ShadowDirectional::ShadowDirectional()
    : m_shaderDepth(Common::ReadFileToString("shaders/vertDepthMap.glsl").c_str(), Common::ReadFileToString("shaders/fragDepthMap.glsl").c_str()),
      m_framebufferDepth(SHADOW_WIDTH, SHADOW_HEIGHT)
{

}

void ShadowDirectional::SetLightPosition(const glm::vec3 & lightPosition)
{
    glm::mat4 lightView = glm::lookAt(lightPosition, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
    m_lightSpaceMatrix = LIGHT_PROJECTION * lightView;
}

void ShadowDirectional::BeginRender()
{
    m_framebufferDepth.BeginRender();

    // viewport must be set after binding framebuffer
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glClear(GL_DEPTH_BUFFER_BIT);

    m_shaderDepth.BeginRender();
    m_shaderDepth.SetUniform(m_lightSpaceMatrix, "lightSpaceMatrix");

    glCullFace(GL_FRONT);
}

void ShadowDirectional::EndRender()
{
    glCullFace(GL_BACK);

    m_shaderDepth.EndRender();

    m_framebufferDepth.EndRender();

    glViewport(0, 0, Common::GetWindowWidth(), Common::GetWindowHeight());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

Shader & ShadowDirectional::GetShader()
{
    return m_shaderDepth;
}

GLuint ShadowDirectional::GetTexture()
{
    return m_framebufferDepth.GetTextureAttachment();
}

glm::vec2 ShadowDirectional::GetTextureSize()
{
    return glm::vec2((float)SHADOW_WIDTH, (float)SHADOW_HEIGHT);
}

const glm::mat4 ShadowDirectional::GetLightSpaceMatrix()
{
    return m_lightSpaceMatrix;
}
