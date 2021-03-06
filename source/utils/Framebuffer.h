#pragma once
#include <stdint.h>
#include "OpenGL.h"
#include <tuple>
#include "EventDispatchers.h"

class Framebuffer : public ResizeReceiver
{
public:
    Framebuffer(uint32_t samples = 0); // create framebuffer equal to window size
    Framebuffer(uint32_t width, uint32_t height, uint32_t samples = 0);
    ~Framebuffer();

    void BeginRender();
    void EndRender();

    GLuint GetTextureAttachment();

private:
    virtual void WindowResized(int32_t width, int32_t height) override;

    void Init();
    void Deinit();

    // return framebuffer, texture
    static std::tuple<GLuint, GLuint> GenerateFramebuffer(uint32_t width, uint32_t height);
    static std::tuple<GLuint, GLuint> GenerateMultisampleFramebuffer(uint32_t width, uint32_t height, uint32_t samples);

    uint32_t m_width = 0;
    uint32_t m_height = 0;
    const uint32_t m_samples = 0;

    GLuint m_framebuffer;
    GLuint m_texture;
    GLint m_originalFramebuffer;

    GLuint m_multisampleFramebuffer;
    GLuint m_multisampleTexture;
};

class FramebufferDepth
{
public:
    FramebufferDepth(uint32_t width, uint32_t height);
    ~FramebufferDepth();

    void BeginRender();
    void EndRender();

    GLuint GetTextureAttachment();
private:
    GLuint m_framebuffer;
    GLuint m_texture;
    GLint m_originalFramebuffer;
};

#if !defined(EMSCRIPTEN) && !defined(ANDROID)
class FramebufferDepthCube
{
public:
    FramebufferDepthCube(uint32_t width, uint32_t height);
    ~FramebufferDepthCube();

    void BeginRender();
    void EndRender();

    GLuint GetTextureAttachment();
private:
    GLuint m_framebuffer;
    GLuint m_texture;
    GLint m_originalFramebuffer;
};
#else
class FramebufferDepthCube
{
public:
    FramebufferDepthCube(uint32_t width, uint32_t height) {}
    ~FramebufferDepthCube() {}
    void BeginRender() {}
    void EndRender() {}
    GLuint GetTextureAttachment() { return 0; }
};
#endif
