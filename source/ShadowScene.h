#pragma once
#include "Shader.h"
#include "Shadow.h"

//#define SHADOW_DIRECTIONAL

class ShadowScene
{
public:
    ShadowScene();

    void Draw(const glm::mat4 & view, const glm::mat4 & projection, const glm::vec3 & cameraPosition, const glm::vec3 & lightPosition);

private:
    void InitPlane();
    void InitCube();
    std::tuple<GLuint, GLuint> InitCommon(const float * data, size_t size);

    void DrawLight(const glm::mat4 & view, const glm::mat4 & projection, const glm::vec3 & cameraPosition, const glm::vec3 & lightPosition);
    void DrawDepth();
    void DrawScenePlane(Shader & shader);
    void DrawSceneInside(Shader & shader);
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

#ifdef SHADOW_DIRECTIONAL
    ShadowDirectional m_shadow;
#else
    ShadowPositional m_shadow;
#endif
};
