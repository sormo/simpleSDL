#pragma once
#include "Shader.h"
#include "Framebuffer.h"

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

private:
    FramebufferDepth m_framebufferDepth;
    Shader m_shaderDepth;

    glm::mat4 m_lightSpaceMatrix;
};
