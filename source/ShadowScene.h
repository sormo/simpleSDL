#pragma once
#include "Shader.h"
#include "Shadow.h"
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
    void DrawScene(Shader & shader);
    void DrawCommon(GLuint vao, GLuint vbo, GLsizei count);

    GLuint m_vaoPlane;
    GLuint m_vboPlane;

    GLuint m_vaoCube;
    GLuint m_vboCube;

    GLuint m_texture;

    Shader m_shaderLight;

    void InitLocations();

    GLuint m_locationPosition;
    GLuint m_locationNormal;
    GLuint m_locationVertexUV;

    Texture::DebugDraw m_debug;

    ShadowDirectional m_shadow;
};
