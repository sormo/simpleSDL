#pragma once
#include "Framebuffer.h"
#include "Shader.h"
#include <vector>

class Postprocess
{
public:
    enum class Type
    {
        NoChange,
        InvertColors,
        Grayscale,
        KernelSharpen,
        KernelBlur,
        KernelEdge
    };

    Postprocess(Type type);

    void BeginRender();
    void EndRender();

private:
    std::vector<float> m_kernel;

    void InitLocations();
    void InitBuffers();
    void Draw();

    Framebuffer m_framebuffer;
    Shader m_shader;

    GLuint m_vao;
    GLuint m_vbo;

    GLuint m_locationPosition;
    GLuint m_locationVertexUV;
    GLuint m_locationTexture;
    GLuint m_locationKernel;
};
