#pragma once
#include "utils/Shader.h"
#include "utils/Framebuffer.h"
#include "utils/Texture.h"
#include <vector>

class ShadowDirectionalLight
{
public:
    ShadowDirectionalLight();

    void SetLightData(const glm::vec3 & direction);

    void BeginRender();
    void EndRender();

    Shader & GetShader();

    GLuint GetTexture();
    glm::vec2 GetTextureSize();
    const glm::mat4 GetLightSpaceMatrix();

    glm::vec2 GetPlanes();

    void DrawDebug();

private:
    FramebufferDepth m_framebufferDepth;
    Shader m_shaderDepth;

    glm::mat4 m_lightSpaceMatrix;

    Texture::DebugDraw m_debug;

    const glm::vec2 m_planes;
    const glm::mat4 m_lightProjection;
};

class ShadowSpotLight
{
public:
    ShadowSpotLight();

    void SetLightData(const glm::vec3 & position, const glm::vec3 & direction, float cutoff, float outerCutoff);

    void BeginRender();
    void EndRender();

    Shader & GetShader();

    GLuint GetTexture();
    glm::vec2 GetTextureSize();
    const glm::mat4 GetLightSpaceMatrix();

    glm::vec2 GetPlanes();

    void DrawDebug();

private:
    FramebufferDepth m_framebufferDepth;
    Shader m_shaderDepth;

    glm::mat4 m_lightSpaceMatrix;

    Texture::DebugDraw m_debug;

    const glm::vec2 m_planes;
    glm::mat4 m_projection;
    glm::mat4 m_view;
};

#if !defined(EMSCRIPTEN) && !defined(ANDROID)
class ShadowPointLight
{
public:
    ShadowPointLight();

    void SetLightData(const glm::vec3 & position);

    void BeginRender();
    void EndRender();

    Shader & GetShader();

    GLuint GetTexture();
    glm::vec2 GetTextureSize();

    // return near and far plane of frustum
    glm::vec2 GetPlanes();

private:
    FramebufferDepthCube m_framebufferDepth;
    Shader m_shaderDepth;

    std::vector<glm::mat4> m_lightSpaceMatrix;
    glm::vec3 m_lightPosition;

    const glm::vec2 m_planes;
    const glm::mat4 m_shadowProjection;
};
#else
// TODO implementation with 6 render passes
class ShadowPointLight
{
public:
    ShadowPointLight() : m_shader("", "") {}
    void SetLightData(const glm::vec3 & position) {}
    void BeginRender() {}
    void EndRender() {}
    Shader & GetShader() { return m_shader; }
    GLuint GetTexture() { return 0; }
    glm::vec2 GetTextureSize() { return {}; }
    glm::vec2 GetPlanes() { return {}; }
private:
    Shader m_shader;
};
#endif
