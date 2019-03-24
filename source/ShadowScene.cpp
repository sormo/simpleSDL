#include "ShadowScene.h"
#include "OpenGL.h"
#include "Common.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

static const float VERTICES_PLANE[] =
{
    // positions            // normals         // texcoords
     25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
    -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,
    -25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,

     25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
     25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,  25.0f, 25.0f,
    -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f
};

static const float VERTICES_CUBE[] = 
{
    // back face
    -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
     1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
     1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
     1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
    -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
    -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
    // front face
    -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
     1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
     1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
     1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
    -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
    -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
    // left face
    -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
    -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
    -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
    -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
    -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
    -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
    // right face
     1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
     1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
     1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
     1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
     1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
     1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
    // bottom face
    -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
     1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
     1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
     1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
    -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
    -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
    // top face
    -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
     1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
     1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
     1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
    -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
    -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
};

ShadowScene::ShadowScene()
#ifdef SHADOW_DIRECTIONAL
    : m_shaderLight(Common::ReadFileToString("shaders/vertNormalShadow.glsl").c_str(), Common::ReadFileToString("shaders/fragDiffPosShadowDirectional.glsl").c_str())
#elif SHADOW_POINT
    : m_shaderLight(Common::ReadFileToString("shaders/vertNormalShadowPositional.glsl").c_str(), Common::ReadFileToString("shaders/fragDiffPosShadowPositional.glsl").c_str())
#else
    : m_shaderLight(Common::ReadFileToString("shaders/vertNormalShadow.glsl").c_str(), Common::ReadFileToString("shaders/fragDiffPosShadowSpot.glsl").c_str())
#endif
{
    InitLocations();
    InitPlane();
    InitCube();

    m_texture = *Texture::Load("wood.png");
}

void ShadowScene::InitLocations()
{
    m_locationPosition = m_shaderLight.GetLocation("positionModelSpace", Shader::LocationType::Attrib);
    m_locationNormal = m_shaderLight.GetLocation("normalModelSpace", Shader::LocationType::Attrib);
    m_locationVertexUV = m_shaderLight.GetLocation("vertexUV", Shader::LocationType::Attrib);
}

std::tuple<GLuint, GLuint> ShadowScene::InitCommon(const float * data, size_t size)
{
    GLuint vao = 0, vbo = 0;

    if (IsVAOSupported())
    {
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
    }

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);

    if (IsVAOSupported())
    {
        glEnableVertexAttribArray(m_locationPosition);
        glVertexAttribPointer(m_locationPosition, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(m_locationNormal);
        glVertexAttribPointer(m_locationNormal, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(m_locationVertexUV);
        glVertexAttribPointer(m_locationVertexUV, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

        glBindVertexArray(0);
    }

    return { vao, vbo };
}

void ShadowScene::InitPlane()
{
    std::tie(m_vaoPlane, m_vboPlane) = InitCommon(VERTICES_PLANE, sizeof(VERTICES_PLANE));
}

void ShadowScene::InitCube()
{
    std::tie(m_vaoCube, m_vboCube) = InitCommon(VERTICES_CUBE, sizeof(VERTICES_CUBE));
}

void ShadowScene::DrawScenePlane(Shader & shader)
{
    auto drawCube = [this](Shader & shader, const glm::mat4 & model)
    {
        shader.SetUniform(model, "model");
        DrawCommon(m_vaoCube, m_vboCube, 36);
    };

    auto drawPlane = [this](Shader & shader, const glm::mat4 & model)
    {
        shader.SetUniform(model, "model");
        DrawCommon(m_vaoPlane, m_vboPlane, 6);
    };

    glm::mat4 model = glm::mat4(1.0f);
    drawPlane(shader, model);

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0));
    model = glm::scale(model, glm::vec3(0.5f));
    drawCube(shader, model);

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0));
    model = glm::scale(model, glm::vec3(0.5f));
    drawCube(shader, model);

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 2.0));
    model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
    model = glm::scale(model, glm::vec3(0.25));
    drawCube(shader, model);
}

void ShadowScene::DrawSceneInside(Shader & shader)
{
    auto drawCube = [this](Shader & shader, const glm::mat4 & model)
    {
        shader.SetUniform(model, "model");
        DrawCommon(m_vaoCube, m_vboCube, 36);
    };

    // room cube
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(5.0f));
    // note that we disable culling here since we render 'inside' the cube instead of the usual 'outside' which throws off the normal culling methods.
    glDisable(GL_CULL_FACE);
    shader.SetUniform(1, "reverseNormals"); // A small little hack to invert normals when drawing cube from the inside so lighting still works.
    drawCube(shader, model);
    shader.SetUniform(0, "reverseNormals"); // and of course disable it
    glEnable(GL_CULL_FACE);

    // cubes
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(4.0f, -3.5f, 0.0));
    model = glm::scale(model, glm::vec3(0.5f));
    drawCube(shader, model);

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(2.0f, 3.0f, 1.0));
    model = glm::scale(model, glm::vec3(0.75f));
    drawCube(shader, model);

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-3.0f, -1.0f, 0.0));
    model = glm::scale(model, glm::vec3(0.5f));
    drawCube(shader, model);

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-1.5f, 1.0f, 1.5));
    model = glm::scale(model, glm::vec3(0.5f));
    drawCube(shader, model);

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-1.5f, 2.0f, -3.0));
    model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
    model = glm::scale(model, glm::vec3(0.75f));
    drawCube(shader, model);
}

void ShadowScene::DrawLight(const glm::mat4 & view, const glm::mat4 & projection, const glm::vec3 & cameraPosition, const glm::vec3 & lightPosition, const glm::vec3 & lightDirection, float cutoff, float outerCutoff)
{
    m_shaderLight.BeginRender();

#ifdef SHADOW_DIRECTIONAL
    m_shaderLight.BindTexture(m_shadow.GetTexture(), "depthMap");
    m_shaderLight.SetUniform(m_shadow.GetTextureSize(), "depthMapSize");
    m_shaderLight.SetUniform(m_shadow.GetLightSpaceMatrix(), "lightSpaceMatrix");
#elif defined(SHADOW_SPOT)
    m_shaderLight.BindTexture(m_shadow.GetTexture(), "depthMap");
    m_shaderLight.SetUniform(m_shadow.GetTextureSize(), "depthMapSize");
    m_shaderLight.SetUniform(m_shadow.GetLightSpaceMatrix(), "lightSpaceMatrix");

    //m_shaderLight.SetUniform(lightPosition, "light.position");
    m_shaderLight.SetUniform(lightDirection, "light.direction");
    m_shaderLight.SetUniform(cutoff, "light.cutoff");
    m_shaderLight.SetUniform(outerCutoff, "light.outerCutoff");
    
    m_shaderLight.SetUniform(1.0f, "light.constant");
    m_shaderLight.SetUniform(0.03f, "light.linear");
    m_shaderLight.SetUniform(0.006f, "light.quadratic");
#else 
    m_shaderLight.BindCubemapTexture(m_shadow.GetTexture(), "depthMap");
    m_shaderLight.SetUniform(m_shadow.GetTextureSize(), "depthMapSize");
    m_shaderLight.SetUniform(m_shadow.GetPlanes().y, "farPlane");
#endif

    m_shaderLight.BindTexture(m_texture, "textureDiffuse");
    m_shaderLight.SetUniform(64.0f, "shininess");
    m_shaderLight.SetUniform(cameraPosition, "cameraWorldSpace");
    m_shaderLight.SetUniform(lightPosition, "light.position");
    m_shaderLight.SetUniform(glm::vec3(0.1f, 0.1f, 0.1f), "light.ambient");
    m_shaderLight.SetUniform(glm::vec3(1.0f, 1.0f, 1.0f), "light.diffuse");
    m_shaderLight.SetUniform(glm::vec3(0.1f, 0.1f, 0.1f), "light.specular");

    m_shaderLight.SetUniform(view, "view");
    m_shaderLight.SetUniform(projection, "projection");

#if defined(SHADOW_DIRECTIONAL) || defined(SHADOW_SPOT)
    DrawScenePlane(m_shaderLight);
#else
    DrawSceneInside(m_shaderLight);
#endif

    m_shaderLight.EndRender();
}

void ShadowScene::DrawDepth()
{
    // render scene from light's point of view
    m_shadow.BeginRender();

#if defined(SHADOW_DIRECTIONAL) || defined(SHADOW_SPOT)
    DrawScenePlane(m_shadow.GetShader());
#else
    DrawSceneInside(m_shadow.GetShader());
#endif

    m_shadow.EndRender();
}

void ShadowScene::DrawCommon(GLuint vao, GLuint vbo, GLsizei count)
{
    if (IsVAOSupported())
    {
        glBindVertexArray(vao);
    }
    else
    {
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        glEnableVertexAttribArray(m_locationPosition);
        glVertexAttribPointer(m_locationPosition, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        
        glEnableVertexAttribArray(m_locationNormal);
        glVertexAttribPointer(m_locationNormal, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        
        glEnableVertexAttribArray(m_locationVertexUV);
        glVertexAttribPointer(m_locationVertexUV, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    }

    glDrawArrays(GL_TRIANGLES, 0, count);
    CheckGlError("glDrawArrays");

    if (IsVAOSupported())
    {
        glBindVertexArray(0);
    }
    else
    {
        glDisableVertexAttribArray(m_locationPosition);
        glDisableVertexAttribArray(m_locationNormal);
        glDisableVertexAttribArray(m_locationVertexUV);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ShadowScene::Draw(const glm::mat4 & view, const glm::mat4 & projection, const glm::vec3 & cameraPosition, const glm::vec3 & lightPosition)
{
    Draw(view, projection, cameraPosition, lightPosition, {}, 0.0f, 0.0f);
}

void ShadowScene::Draw(const glm::mat4 & view, const glm::mat4 & projection, const glm::vec3 & cameraPosition, const glm::vec3 & lightPosition, const glm::vec3 & lightDirection, float cutoff, float outerCutoff)
{
#ifdef SHADOW_SPOT
    m_shadow.SetLightData(lightPosition, lightDirection, cutoff, outerCutoff);
#else
    m_shadow.SetLightData(lightPosition);
#endif

    DrawDepth();

    m_shadow.DrawDebug();
    //DrawLight(view, projection, cameraPosition, lightPosition, lightDirection, cutoff, outerCutoff);
}
