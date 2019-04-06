#include "BulletDebug.h"

static const char VERTEX_SHADER[] = \
"#version 100\n"
"#ifdef GL_ES\n"
"precision mediump float;\n"
"#endif\n"
"attribute vec3 positionWorldSpace;\n"
"attribute vec3 color;\n"
"varying vec3 acolor;\n"
"uniform mat4 VP;\n"
"void main()\n"
"{\n"
"    acolor = color;\n"
"    gl_Position = VP * vec4(positionWorldSpace, 1.0);\n"
"}\n";

static const char FRAGMENT_SHADER[] = \
"#version 100\n"
"#ifdef GL_ES\n"
"precision mediump float;\n"
"#endif\n"
"varying vec3 acolor;\n"
"void main()\n"
"{\n"
"    gl_FragColor = vec4(acolor, 1.0);\n"
"}\n";

BulletDebug::BulletDebug()
    : m_shader(VERTEX_SHADER, FRAGMENT_SHADER)
{
    m_locationVP = m_shader.GetLocation("VP", Shader::LocationType::Uniform);
    m_locationPosition = m_shader.GetLocation("positionWorldSpace", Shader::LocationType::Attrib);
    m_locationColor = m_shader.GetLocation("color", Shader::LocationType::Attrib);
}

BulletDebug::~BulletDebug()
{

}

void BulletDebug::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
{
    m_lineData.push_back({ from.x(), from.y(), from.z() });
    m_lineData.push_back({ to.x(), to.y(), to.z() });

    m_colorData.push_back({ color.x(), color.y(), color.z() });
    m_colorData.push_back({ color.x(), color.y(), color.z() });
}

void BulletDebug::Draw(const glm::mat4 & view, const glm::mat4 & projection)
{
    glDisable(GL_DEPTH_TEST);

    GLuint vbo[2];

    glGenBuffers(2, vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, m_lineData.size() * sizeof(glm::vec3), m_lineData.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, m_colorData.size() * sizeof(glm::vec3), m_colorData.data(), GL_STATIC_DRAW);

    m_shader.BeginRender();

    m_shader.BindBuffer<glm::vec3>(vbo[0], m_locationPosition);
    m_shader.BindBuffer<glm::vec3>(vbo[1], m_locationColor);

    m_shader.SetUniform(projection * view, m_locationVP);

    glDrawArrays(GL_LINES, 0, m_lineData.size());
    CheckGlError("glDrawArrays");

    m_shader.EndRender();

    glDeleteBuffers(2, vbo);

    glEnable(GL_DEPTH_TEST);
}

void BulletDebug::Clear()
{
    m_lineData.clear();
    m_colorData.clear();
}

void BulletDebug::drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
{

}

void BulletDebug::reportErrorWarning(const char* warningString)
{

}

void BulletDebug::draw3dText(const btVector3& location, const char* textString)
{

}

void BulletDebug::setDebugMode(int debugMode)
{
    m_debugMode = debugMode;
}

int BulletDebug::getDebugMode() const
{
    return m_debugMode;
}
