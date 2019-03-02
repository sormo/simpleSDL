#include "ShadowScene.h"
#include "OpenGL.h"
#include "Common.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"


static const glm::vec3 LIGHT_POSITION(-2.0f, 4.0f, -1.0f);
static const GLsizei SHADOW_WIDTH = 1024;
static const GLsizei SHADOW_HEIGHT = 1024;

static const float NEAR_PLANE = 1.0f;
static const float FAR_PLANE = 7.5f;

glm::mat4 LIGHT_PROJECTION = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, NEAR_PLANE, FAR_PLANE);
glm::mat4 LIGHT_VIEW = glm::lookAt(LIGHT_POSITION, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
glm::mat4 LIGHT_SPACE_MATRIX = LIGHT_PROJECTION * LIGHT_VIEW;

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
    : m_shaderLight(Common::ReadFileToString("shaders/vertNormalShadow.glsl").c_str(), Common::ReadFileToString("shaders/fragDiffPosShadow.glsl").c_str()),
      m_shaderDepth(Common::ReadFileToString("shaders/vertDepthMap.glsl").c_str(), Common::ReadFileToString("shaders/fragDepthMap.glsl").c_str()),
      m_framebufferDepth(SHADOW_WIDTH, SHADOW_HEIGHT),
      m_debug(nullptr, Common::ReadFileToString("shaders/fragDepthDebug.glsl").c_str())
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

void ShadowScene::DrawScene(Shader & shader)
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

void ShadowScene::DrawLight(const glm::mat4 & view, const glm::mat4 & projection, const glm::vec3 & cameraPosition)
{
    m_shaderLight.BeginRender();

    m_shaderLight.BindTexture(m_texture, "textureDiffuse");
    m_shaderLight.BindTexture(m_framebufferDepth.GetTextureAttachment(), "depthMap");
    m_shaderLight.SetUniform(64.0f, "shininess");
    m_shaderLight.SetUniform(cameraPosition, "cameraWorldSpace");
    m_shaderLight.SetUniform(LIGHT_POSITION, "light.position");
    m_shaderLight.SetUniform(glm::vec3(0.1f, 0.1f, 0.1f), "light.ambient");
    m_shaderLight.SetUniform(glm::vec3(1.0f, 1.0f, 1.0f), "light.diffuse");
    m_shaderLight.SetUniform(glm::vec3(0.2f, 0.2f, 0.2f), "light.specular");
    m_shaderLight.SetUniform(LIGHT_SPACE_MATRIX, "lightSpaceMatrix");
    m_shaderLight.SetUniform(glm::vec2((float)SHADOW_WIDTH, (float)SHADOW_HEIGHT), "depthMapSize");
    m_shaderLight.SetUniform(view, "view");
    m_shaderLight.SetUniform(projection, "projection");

    DrawScene(m_shaderLight);

    m_shaderLight.EndRender();
}

void ShadowScene::DrawDepth()
{
    // render scene from light's point of view
    
    m_framebufferDepth.BeginRender();

    // viewport must be set after binding framebuffer
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glClear(GL_DEPTH_BUFFER_BIT);

    m_shaderDepth.BeginRender();
    m_shaderDepth.SetUniform(LIGHT_SPACE_MATRIX, "lightSpaceMatrix");
    
    DrawScene(m_shaderDepth);

    m_framebufferDepth.EndRender();

    m_shaderDepth.EndRender();
}

void ShadowScene::DrawCommon(GLuint vao, GLuint vbo, GLsizei count)
{
    if (IsVAOSupported())
    {
        glBindVertexArray(vao);
    }
    else
    {
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
}

void ShadowScene::Draw(const glm::mat4 & view, const glm::mat4 & projection, const glm::vec3 & cameraPosition)
{
    DrawDepth();

    glViewport(0, 0, Common::GetWindowWidth(), Common::GetWindowHeight());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    DrawLight(view, projection, cameraPosition);

    //m_debug.Draw(m_framebufferDepth.GetTextureAttachment(), [](Shader & shader)
    //{
    //    shader.SetUniform(NEAR_PLANE, "nearPlane");
    //    shader.SetUniform(FAR_PLANE, "farPlane");
    //});
}
