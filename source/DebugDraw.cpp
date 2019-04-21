#include "DebugDraw.h"

static const char LINE_VERTEX_SHADER[] = \
"#version 100\n"
"#ifdef GL_ES\n"
"precision mediump float;\n"
"#endif\n"
"attribute vec3 positionWorldSpace;\n"
"attribute vec4 color;\n"
"varying vec4 acolor;\n"
"uniform mat4 VP;\n"
"void main()\n"
"{\n"
"    acolor = color;\n"
"    gl_Position = VP * vec4(positionWorldSpace, 1.0);\n"
"    gl_PointSize = 5.0;\n"
"}\n";

static const char LINE_FRAGMENT_SHADER[] = \
"#version 100\n"
"#ifdef GL_ES\n"
"precision mediump float;\n"
"#endif\n"
"varying vec4 acolor;\n"
"void main()\n"
"{\n"
"    gl_FragColor = acolor;\n"
"}\n";

DebugDraw::DebugDraw()
    : m_shader(LINE_VERTEX_SHADER, LINE_FRAGMENT_SHADER)
{
    m_locationVP = m_shader.GetLocation("VP", Shader::LocationType::Uniform);
    m_locationPosition = m_shader.GetLocation("positionWorldSpace", Shader::LocationType::Attrib);
    m_locationColor = m_shader.GetLocation("color", Shader::LocationType::Attrib);
}

void DebugDraw::DrawPoint(const glm::vec3& center, const glm::vec4& color)
{
    m_pointData.push_back(center);
    m_pointColor.push_back(color);
}

void DebugDraw::DrawLine(const glm::vec3& from, const glm::vec3& to, const glm::vec4& color)
{
    m_lineData.push_back(from);
    m_lineData.push_back(to);

    m_lineColor.push_back(color);
    m_lineColor.push_back(color);
}

void DebugDraw::DrawTriangle(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, const glm::vec4& color)
{
    m_triangleData.push_back(p1);
    m_triangleData.push_back(p2);
    m_triangleData.push_back(p3);

    m_triangleColor.push_back(color);
    m_triangleColor.push_back(color);
    m_triangleColor.push_back(color);
}

void DebugDraw::DrawPlane(const Common::Math::Plane& plane, const glm::vec4& color)
{

}

void DebugDraw::DrawLine(const Common::Math::Line& line, const glm::vec4& color)
{

}

void DebugDraw::DrawCircle(const glm::vec3& center, float radius, const Common::Math::Plane& plane, const glm::vec4& color)
{

}

void DebugDraw::DrawSquare(const glm::vec3& p1, const glm::vec3& p2, const glm::vec4& color)
{

}

void DebugDraw::Draw(const glm::mat4& view, const glm::mat4& projection)
{
    //glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    DrawPoints(view, projection);
    DrawLines(view, projection);
    DrawTriangles(view, projection);

    glDisable(GL_BLEND);

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
}

void DebugDraw::DrawPoints(const glm::mat4& view, const glm::mat4& projection)
{
    glEnable(GL_PROGRAM_POINT_SIZE);

    DrawCommon(view, projection, GL_POINTS, m_pointData, m_pointColor);

    glDisable(GL_PROGRAM_POINT_SIZE);
}

void DebugDraw::DrawLines(const glm::mat4& view, const glm::mat4& projection)
{
    DrawCommon(view, projection, GL_LINES, m_lineData, m_lineColor);
}

void DebugDraw::DrawTriangles(const glm::mat4& view, const glm::mat4& projection)
{
    DrawCommon(view, projection, GL_TRIANGLES, m_triangleData, m_triangleColor);
}

void DebugDraw::DrawCommon(const glm::mat4& view, const glm::mat4& projection,
    GLenum primitives, const std::vector<glm::vec3>& data, const std::vector<glm::vec4>& colors)
{
    GLuint vbo[2];

    glGenBuffers(2, vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(glm::vec3), data.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec4), colors.data(), GL_STATIC_DRAW);

    m_shader.BeginRender();

    m_shader.BindBuffer<glm::vec3>(vbo[0], m_locationPosition);
    m_shader.BindBuffer<glm::vec4>(vbo[1], m_locationColor);

    m_shader.SetUniform(projection * view, m_locationVP);

    glDrawArrays(primitives, 0, data.size());
    CheckGlError("glDrawArrays");

    m_shader.EndRender();

    glDeleteBuffers(2, vbo);
}

void DebugDraw::Clear()
{
    m_lineData.clear();
    m_lineColor.clear();
}
