#include "Shadow.h"
#include "Common.h"
#include "glm/gtc/matrix_transform.hpp"

static const GLsizei SHADOW_WIDTH = 1024;
static const GLsizei SHADOW_HEIGHT = 1024;

static const char * DIRECTIONAL_FRAGMENT_SHADER = \
"#version 100\n"
"void main()\n"
"{\n"
"    // gl_FragDepth = gl_FragCoord.z;\n"
"}\n";

static const char * DIRECTIONAL_VERTEX_SHADER = \
"#version 100\n"
"#ifdef GL_ES\n"
"precision mediump float;\n"
"#endif\n"
"attribute vec3 positionModelSpace;\n"
"uniform mat4 lightSpaceMatrix;\n"
"uniform mat4 model;\n"
"void main()\n"
"{\n"
"    gl_Position = lightSpaceMatrix * model * vec4(positionModelSpace, 1.0);\n"
"}\n";

static const char * DIRECTIONAL_DEBUG_FRAGMENT_SHADER = \
"#version 100\n"
"#ifdef GL_ES\n"
"precision mediump float;\n"
"#endif\n"
"varying vec2 vertexUVA;\n"
"uniform sampler2D colorMap;\n"
"void main()\n"
"{\n"
"    float depthValue = texture2D(colorMap, vertexUVA).r;\n"
"    gl_FragColor = vec4(vec3(depthValue), 1.0); // orthographic\n"
"}\n";

ShadowDirectionalLight::ShadowDirectionalLight()
    : m_shaderDepth(DIRECTIONAL_VERTEX_SHADER, DIRECTIONAL_FRAGMENT_SHADER),
      m_framebufferDepth(SHADOW_WIDTH, SHADOW_HEIGHT),
      m_debug(nullptr, DIRECTIONAL_DEBUG_FRAGMENT_SHADER),
      m_planes(0.1f, 10.0f),
      m_lightProjection(glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, m_planes.x, m_planes.y))
{

}

void ShadowDirectionalLight::SetLightData(const glm::vec3 & lightPosition)
{
    glm::mat4 lightView = glm::lookAt(lightPosition, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
    m_lightSpaceMatrix = m_lightProjection * lightView;
}

void ShadowDirectionalLight::BeginRender()
{
    m_framebufferDepth.BeginRender();

    // viewport must be set after binding framebuffer
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glClear(GL_DEPTH_BUFFER_BIT);

    m_shaderDepth.BeginRender();
    m_shaderDepth.SetUniform(m_lightSpaceMatrix, "lightSpaceMatrix");

    glCullFace(GL_FRONT);
}

void ShadowDirectionalLight::EndRender()
{
    glCullFace(GL_BACK);

    m_shaderDepth.EndRender();

    m_framebufferDepth.EndRender();

    glViewport(0, 0, Common::GetWindowWidth(), Common::GetWindowHeight());
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

Shader & ShadowDirectionalLight::GetShader()
{
    return m_shaderDepth;
}

GLuint ShadowDirectionalLight::GetTexture()
{
    return m_framebufferDepth.GetTextureAttachment();
}

glm::vec2 ShadowDirectionalLight::GetTextureSize()
{
    return glm::vec2((float)SHADOW_WIDTH, (float)SHADOW_HEIGHT);
}

const glm::mat4 ShadowDirectionalLight::GetLightSpaceMatrix()
{
    return m_lightSpaceMatrix;
}

void ShadowDirectionalLight::DrawDebug()
{
    m_debug.Draw(m_framebufferDepth.GetTextureAttachment());
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char * SPOT_DEBUG_FRAGMENT_SHADER = \
"#version 100\n"
"#ifdef GL_ES\n"
"precision mediump float;\n"
"#endif\n"
"varying vec2 vertexUVA;\n"
"uniform sampler2D colorMap;\n"
"void main()\n"
"{\n"
"    float depthValue = texture2D(colorMap, vertexUVA).r;\n"
"    depthValue = 1.0 - (1.0 - depthValue) * 25.0;"
"    gl_FragColor = vec4(vec3(depthValue), 1.0); // projection\n"
"}\n";

ShadowSpotLight::ShadowSpotLight()
    : m_shaderDepth(DIRECTIONAL_VERTEX_SHADER, DIRECTIONAL_FRAGMENT_SHADER),
    m_framebufferDepth(SHADOW_WIDTH, SHADOW_HEIGHT),
    m_debug(nullptr, SPOT_DEBUG_FRAGMENT_SHADER),
    m_planes(0.1f, 25.0f)
{

}

void ShadowSpotLight::SetLightData(const glm::vec3 & position, const glm::vec3 & direction, float cutoff, float outerCutoff)
{
    m_projection = glm::perspective(2.0f*glm::acos(outerCutoff), (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT, m_planes.x, m_planes.y);
    m_view = glm::lookAt(position, position + direction, glm::vec3(0.0, 1.0, 0.0));
    
    m_lightSpaceMatrix = m_projection * m_view;
}

// looks like all of those functions are same

void ShadowSpotLight::BeginRender()
{
    m_framebufferDepth.BeginRender();

    // viewport must be set after binding framebuffer
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glClear(GL_DEPTH_BUFFER_BIT);

    m_shaderDepth.BeginRender();
    m_shaderDepth.SetUniform(m_lightSpaceMatrix, "lightSpaceMatrix");

    //glCullFace(GL_FRONT);
}

void ShadowSpotLight::EndRender()
{
    //glCullFace(GL_BACK);

    m_shaderDepth.EndRender();

    m_framebufferDepth.EndRender();

    glViewport(0, 0, Common::GetWindowWidth(), Common::GetWindowHeight());
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

Shader & ShadowSpotLight::GetShader()
{
    return m_shaderDepth;
}

GLuint ShadowSpotLight::GetTexture()
{
    return m_framebufferDepth.GetTextureAttachment();
}

glm::vec2 ShadowSpotLight::GetTextureSize()
{
    return glm::vec2((float)SHADOW_WIDTH, (float)SHADOW_HEIGHT);
}

const glm::mat4 ShadowSpotLight::GetLightSpaceMatrix()
{
    return m_lightSpaceMatrix;
}

void ShadowSpotLight::DrawDebug()
{
    m_debug.Draw(m_framebufferDepth.GetTextureAttachment());
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if !defined(EMSCRIPTEN) && !defined(ANDROID)
static const char * POSITIONAL_VERTEX_SHADER = \
"#version 150\n"
"in vec3 positionModelSpace;\n"
"uniform mat4 model;\n"
"void main()\n"
"{\n"
"    gl_Position = model * vec4(positionModelSpace, 1.0);\n"
"}\n";

static const char * POSITIONAL_GEOMETRY_SHADER = \
"#version 150\n"
"layout (triangles) in;\n"
"layout (triangle_strip, max_vertices = 18) out;\n"
"uniform mat4 shadowMatrices[6];\n"
"out vec4 positionLightSpace;\n"
"void main()\n"
"{\n"
"    for(int face = 0; face < 6; ++face)\n"
"    {\n"
"        gl_Layer = face; // built-in variable that specifies to which face we render.\n"
"        for(int i = 0; i < 3; ++i) // for each triangle's vertices\n"
"        {\n"
"            positionLightSpace = gl_in[i].gl_Position;\n"
"            gl_Position = shadowMatrices[face] * positionLightSpace;\n"
"            EmitVertex();\n"
"        }\n"
"        EndPrimitive();\n"
"    }\n"
"}\n";

static const char * POSITIONAL_FRAGMENT_SHADER = \
"#version 150\n"
"in vec4 positionLightSpace;\n"
"uniform vec3 lightPositionWorldSpace;\n"
"uniform float farPlane;\n"
"void main()\n"
"{\n"
"    float lightDistance = length(positionLightSpace.xyz - lightPositionWorldSpace);\n"
"    // map to [0;1] range by dividing by farPlane\n"
"    lightDistance = lightDistance / farPlane;\n"
"    // write this as modified depth\n"
"    gl_FragDepth = lightDistance;\n"
"}\n";

ShadowPointLight::ShadowPointLight()
    : m_shaderDepth(POSITIONAL_VERTEX_SHADER, POSITIONAL_GEOMETRY_SHADER, POSITIONAL_FRAGMENT_SHADER),
      m_framebufferDepth(SHADOW_WIDTH, SHADOW_HEIGHT),
      m_planes(0.1f, 25.0f),
      m_shadowProjection(glm::perspective(glm::radians(90.0f), (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT, m_planes.x, m_planes.y))
{
}

void ShadowPointLight::SetLightData(const glm::vec3 & lightPosition)
{
    m_lightSpaceMatrix.clear();

    m_lightSpaceMatrix.push_back(m_shadowProjection * glm::lookAt(lightPosition, lightPosition + glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)));
    m_lightSpaceMatrix.push_back(m_shadowProjection * glm::lookAt(lightPosition, lightPosition + glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)));
    m_lightSpaceMatrix.push_back(m_shadowProjection * glm::lookAt(lightPosition, lightPosition + glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)));
    m_lightSpaceMatrix.push_back(m_shadowProjection * glm::lookAt(lightPosition, lightPosition + glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)));
    m_lightSpaceMatrix.push_back(m_shadowProjection * glm::lookAt(lightPosition, lightPosition + glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)));
    m_lightSpaceMatrix.push_back(m_shadowProjection * glm::lookAt(lightPosition, lightPosition + glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f)));

    m_lightPosition = lightPosition;
}

void ShadowPointLight::BeginRender()
{
    m_framebufferDepth.BeginRender();

    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glClear(GL_DEPTH_BUFFER_BIT);

    m_shaderDepth.BeginRender();
    m_shaderDepth.SetUniform(m_lightSpaceMatrix[0], "shadowMatrices[0]");
    m_shaderDepth.SetUniform(m_lightSpaceMatrix[1], "shadowMatrices[1]");
    m_shaderDepth.SetUniform(m_lightSpaceMatrix[2], "shadowMatrices[2]");
    m_shaderDepth.SetUniform(m_lightSpaceMatrix[3], "shadowMatrices[3]");
    m_shaderDepth.SetUniform(m_lightSpaceMatrix[4], "shadowMatrices[4]");
    m_shaderDepth.SetUniform(m_lightSpaceMatrix[5], "shadowMatrices[5]");

    m_shaderDepth.SetUniform(m_planes.y, "farPlane");
    m_shaderDepth.SetUniform(m_lightPosition, "lightPositionWorldSpace");
}

void ShadowPointLight::EndRender()
{
    m_shaderDepth.EndRender();

    m_framebufferDepth.EndRender();

    glViewport(0, 0, Common::GetWindowWidth(), Common::GetWindowHeight());
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

Shader & ShadowPointLight::GetShader()
{
    return m_shaderDepth;
}

GLuint ShadowPointLight::GetTexture()
{
    return m_framebufferDepth.GetTextureAttachment();
}

glm::vec2 ShadowPointLight::GetTextureSize()
{
    return glm::vec2((float)SHADOW_WIDTH, (float)SHADOW_HEIGHT);
}

glm::vec2 ShadowPointLight::GetPlanes()
{
    return m_planes;
}
#endif
