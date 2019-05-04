#include "Framebuffer.h"
#include "Common.h"
#include "OpenGL.h"
#include "Texture.h"
#include "EventDispatchers.h"

namespace Application
{
    extern ResizeDispatcher g_resizeDispatcher;
}

void CheckRenderBufferSize(uint32_t width, uint32_t height)
{
    GLint maxRenderbufferSize;
    glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &maxRenderbufferSize);

    if (maxRenderbufferSize < (GLint)width || maxRenderbufferSize < (GLint)height)
    {
        printf("Error creating framebuffer %dx%x, GL_MAX_RENDERBUFFER_SIZE is %d", width, height, maxRenderbufferSize);
        throw std::runtime_error("Error creating framebuffer.");
    }
}

GLenum GetOverridenDataTypeOfPixel(GLenum requested)
{
#if defined(ANDROID) || defined(EMSCRIPTEN)
    //if (requested == GL_FLOAT && !IsOpenGlExtensionSupported("GL_OES_texture_float"))
    //{
    //    printf("Attention, requsted pixel type not supprted (0x%X) falling back to GL_UNSIGNED_SHORT", requested);
    //    return GL_UNSIGNED_SHORT;
    //}
    return GL_UNSIGNED_SHORT;
#endif
    return requested;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Framebuffer::Framebuffer(uint32_t samples)
    : Framebuffer(Common::GetWindowWidth(), Common::GetWindowHeight(), samples)
{
    Application::g_resizeDispatcher.Add(this);
}

Framebuffer::Framebuffer(uint32_t width, uint32_t height, uint32_t samples)
    : m_width(width), m_height(height), m_samples(samples)
{
    Init();
}

Framebuffer::~Framebuffer()
{
    Application::g_resizeDispatcher.Remove(this);

    Deinit();
}

void Framebuffer::WindowResized(int32_t width, int32_t height)
{
    Deinit();

    m_width = width;
    m_height = height;

    Init();
}

void Framebuffer::Init()
{
    CheckRenderBufferSize(m_width, m_height);

    std::tie(m_framebuffer, m_texture) = GenerateFramebuffer(m_width, m_height);

#if !defined(EMSCRIPTEN) && !defined(ANDROID)
    if (m_samples)
    {
        std::tie(m_multisampleFramebuffer, m_multisampleTexture) = GenerateMultisampleFramebuffer(m_width, m_height, m_samples);
    }
#endif
}

void Framebuffer::Deinit()
{
    glDeleteTextures(1, &m_texture);
    glDeleteFramebuffers(1, &m_framebuffer);
    if (m_samples)
    {
        glDeleteTextures(1, &m_multisampleTexture);
        glDeleteFramebuffers(1, &m_multisampleFramebuffer);
    }
}

void Framebuffer::BeginRender()
{
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &m_originalFramebuffer);

#if !defined(EMSCRIPTEN) && !defined(ANDROID)
    if (m_samples)
        glBindFramebuffer(GL_FRAMEBUFFER, m_multisampleFramebuffer);
    else
#endif
        glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);

    CheckGlError("glBindFramebuffer");
}

void Framebuffer::EndRender()
{
#if !defined(EMSCRIPTEN) && !defined(ANDROID)
    if (m_samples)
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, m_multisampleFramebuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_framebuffer);
        glBlitFramebuffer(0, 0, m_width, m_height, 0, 0, m_width, m_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

        CheckGlError("glBlitFramebuffer");
    }
#endif

    glBindFramebuffer(GL_FRAMEBUFFER, m_originalFramebuffer);
}

GLuint Framebuffer::GetTextureAttachment()
{
    return m_texture;
}

std::tuple<GLuint, GLuint> Framebuffer::GenerateFramebuffer(uint32_t width, uint32_t height)
{
    GLuint framebuffer, texture;

    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, Texture::GetCorrectWrapMode(GL_REPEAT, width));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, Texture::GetCorrectWrapMode(GL_REPEAT, height));

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

    // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
    GLuint renderBufferObject;

    glGenRenderbuffers(1, &renderBufferObject);
    glBindRenderbuffer(GL_RENDERBUFFER, renderBufferObject);

#if defined(ANDROID)
    // TODO opengl es 2
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8_OES, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderBufferObject);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBufferObject);
#elif defined(EMSCRIPTEN)
    // TODO don't know why it don't work with GL_DEPTH24_STENCIL8 on webgl
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderBufferObject);
#else
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBufferObject);
#endif

    // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        printf("Error initializing frame buffer.\n");
        throw std::runtime_error("Error initializing frame buffer.");
    }

    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return { framebuffer, texture };
}

#if !defined(EMSCRIPTEN) && !defined(ANDROID)
std::tuple<GLuint, GLuint> Framebuffer::GenerateMultisampleFramebuffer(uint32_t width, uint32_t height, uint32_t samples)
{
    GLuint framebuffer, texture;

    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texture);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGBA, width, height, GL_TRUE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, Texture::GetCorrectWrapMode(GL_REPEAT, width));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, Texture::GetCorrectWrapMode(GL_REPEAT, height));

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, texture, 0);

    // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
    GLuint renderBufferObject;

    glGenRenderbuffers(1, &renderBufferObject);
    glBindRenderbuffer(GL_RENDERBUFFER, renderBufferObject);

#if defined(ANDROID)
    // TODO opengl es 2
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH24_STENCIL8_OES, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderBufferObject);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBufferObject);
#elif defined(EMSCRIPTEN)
    // TODO don't know why it don't work with GL_DEPTH24_STENCIL8 on webgl
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH_COMPONENT16, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderBufferObject);
#else
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBufferObject);
#endif

    // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        printf("Error initializing frame buffer.\n");
        throw std::runtime_error("Error initializing frame buffer.");
    }

    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return { framebuffer, texture };
}
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

FramebufferDepth::FramebufferDepth(uint32_t width, uint32_t height)
{
    CheckRenderBufferSize(width, height);

    glGenFramebuffers(1, &m_framebuffer);

    // prepare depth texture
    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GetOverridenDataTypeOfPixel(GL_FLOAT), NULL);
    CheckGlError("glTexImage2D");

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

#if !defined(ANDROID) && !defined(EMSCRIPTEN)
    static const float BORDER_COLOR[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, BORDER_COLOR);
#endif

    glBindTexture(GL_TEXTURE_2D, 0);

    // attach depth texture as FBO's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
    CheckGlError("glBindFramebuffer");

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_texture, 0);
    CheckGlError("glFramebufferTexture2D");

#if defined(EMSCRIPTEN)
    // for webgl create color attachment
    GLuint renderBufferObject;
    glGenRenderbuffers(1, &renderBufferObject);

    glBindRenderbuffer(GL_RENDERBUFFER, renderBufferObject);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA4, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, renderBufferObject);
#elif ANDROID
    // for android nothing ???
#else
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
#endif
    
    // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        printf("Error initializing frame buffer.\n");
        throw std::runtime_error("Error initializing frame buffer.");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

FramebufferDepth::~FramebufferDepth()
{
    glDeleteTextures(1, &m_texture);
    glDeleteFramebuffers(1, &m_framebuffer);
}

void FramebufferDepth::BeginRender()
{
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &m_originalFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
}

void FramebufferDepth::EndRender()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_originalFramebuffer);
}

GLuint FramebufferDepth::GetTextureAttachment()
{
    return m_texture;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if !defined(EMSCRIPTEN) && !defined(ANDROID)
FramebufferDepthCube::FramebufferDepthCube(uint32_t width, uint32_t height)
{
    CheckRenderBufferSize(width, height);

    glGenFramebuffers(1, &m_framebuffer);

    // prepare depth texture
    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture);

    for (uint32_t i = 0; i < 6; ++i)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GetOverridenDataTypeOfPixel(GL_FLOAT), NULL);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    // attach depth texture as FBO's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
    // this is special from opengl 3.1
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_texture, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

FramebufferDepthCube::~FramebufferDepthCube()
{
    glDeleteTextures(1, &m_texture);
    glDeleteFramebuffers(1, &m_framebuffer);
}

void FramebufferDepthCube::BeginRender()
{
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &m_originalFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
}

void FramebufferDepthCube::EndRender()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_originalFramebuffer);
}

GLuint FramebufferDepthCube::GetTextureAttachment()
{
    return m_texture;
}
#endif
