#pragma once
#include <optional>
#include <vector>
#include <functional>
#include "OpenGL.h"
#include "Shader.h"

namespace Texture
{
    std::optional<GLuint> LoadBMP(const char * imagePath);
    std::optional<GLuint> LoadDDS(const char * imagePath);

    bool WriteTGA(const char* imagePath, GLuint texture);

    // load image using STB
    std::optional<GLuint> Load(const char * imagePath);

    // load cubemap
    // textures in following order: right, left, top, bottom, back, front
    std::optional<GLuint> LoadCubemap(const std::vector<std::string> & paths);

    // warning: emscripten accepts only GL_CLAMP_TO_EDGE for non-power of two textures
    GLint GetCorrectWrapMode(GLint desired, int32_t size);

    // expects colorMap sampler in fragment shader
    class DebugDraw
    {
    public:
        DebugDraw();
        DebugDraw(const char * vertexShader, const char * fragmentShader);
        ~DebugDraw();

        void Draw(GLuint texture, std::function<void(Shader & shader)> bindCallback = nullptr);
    private:
        GLuint m_vaoQuad;
        GLuint m_vboQuad;
        Shader m_shader;
    };
}
