#pragma once
#include "Shader.h"
#include "Framebuffer.h"
#include "Texture.h"
#include <vector>

class ShadowDirectional
{
public:
    ShadowDirectional();

    void SetLightPosition(const glm::vec3 & lightPosition);

    void BeginRender();
    void EndRender();

    Shader & GetShader();

    GLuint GetTexture();
    glm::vec2 GetTextureSize();
    const glm::mat4 GetLightSpaceMatrix();

    void DrawDebug();

private:
    FramebufferDepth m_framebufferDepth;
    Shader m_shaderDepth;

    glm::mat4 m_lightSpaceMatrix;

    Texture::DebugDraw m_debug;

    const glm::vec2 m_planes;
    const glm::mat4 m_lightProjection;
};

class ShadowPositional
{
public:
    ShadowPositional();

    void SetLightPosition(const glm::vec3 & lightPosition);

    void BeginRender();
    void EndRender();

    Shader & GetShader();

    GLuint GetTexture();
    glm::vec2 GetTextureSize();

    // return near and far plane of frustrum
    glm::vec2 GetPlanes();

private:
    FramebufferDepthCube m_framebufferDepth;
    Shader m_shaderDepth;

    std::vector<glm::mat4> m_lightSpaceMatrix;
    glm::vec3 m_lightPosition;

    const glm::vec2 m_planes;
    const glm::mat4 m_shadowProjection;
};
