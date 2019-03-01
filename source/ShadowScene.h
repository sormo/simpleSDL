#pragma once
#include "Shader.h"
#include "Framebuffer.h"
#include "Texture.h"

class ShadowScene
{
public:
    ShadowScene();

    void Draw(const glm::mat4 & view, const glm::mat4 & projection, const glm::vec3 & cameraPosition);

private:
    void InitPlane();
    void InitCube();
    std::tuple<GLuint, GLuint> InitCommon(const float * data, size_t size);

    void DrawLight(const glm::mat4 & view, const glm::mat4 & projection, const glm::vec3 & cameraPosition);
    void DrawDepth();
    void DrawScene(const std::function<void(const glm::mat4 &)> & bindFunction);
    void DrawCommon(GLuint vao, GLuint vbo, GLsizei count);

    GLuint m_texture;

    GLuint m_vaoPlane;
    GLuint m_vboPlane;

    GLuint m_vaoCube;
    GLuint m_vboCube;

    Shader m_shaderLight;
    Shader m_shaderDepth;

    void InitLocations();

    GLuint m_locationPosition;
    GLuint m_locationNormal;
    GLuint m_locationVertexUV;

    FramebufferDepth m_framebufferDepth;

    Texture::DebugDraw m_debug;
};
