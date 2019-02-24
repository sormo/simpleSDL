#pragma once
#include <stdint.h>
#include "OpenGL.h"

class Framebuffer
{
public:
    Framebuffer(); // create framebuffer equal to window size
    Framebuffer(uint32_t width, uint32_t height);
    ~Framebuffer();

    void BeginRender();
    void EndRender();

    GLuint GetTextureAttachment();

private:
    GLuint m_framebuffer;
    GLuint m_texture;
};
