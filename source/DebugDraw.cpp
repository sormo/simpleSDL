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

void DebugDraw::DrawRectangle(const glm::vec3& center, const glm::vec2& halfExtents, const glm::vec3& planeNormal, const glm::vec4& color)
{
    auto SolveZ = [](const glm::vec2 & v, const glm::vec3 & n)
    {
        return (-n.x * v.x - n.y * v.y) / n.z;
    };

    auto GetUpVector = [](const glm::vec3 & normal, const glm::vec2 & halfExtents)
    {
        if (normal.z != 0.0f)
            return glm::vec3(0.0f, halfExtents.y, -normal.y * halfExtents.y / normal.z);
        else if (normal.y != 0.0f)
            return glm::vec3(halfExtents.x, -normal.x * halfExtents.x / normal.y, 0.0f);
        else if (normal.x != 0.0f)
            return glm::vec3(-normal.y * halfExtents.y / normal.x, halfExtents.y, 0.0f);
        return glm::vec3{ 0.0f, 1.0f, 0.0f };
    };

    auto normal = glm::normalize(planeNormal);

    glm::vec3 upVectorPlane = GetUpVector(normal, halfExtents);

    DrawLine(center, center + upVectorPlane, { 1.0f, 1.0f, 1.0f, 1.0f });

    glm::vec3 rightVectorPlane = glm::cross(normal, upVectorPlane);

    rightVectorPlane = rightVectorPlane * (halfExtents.x / halfExtents.y);

    DrawLine(center, center + rightVectorPlane, { 1.0f, 1.0f, 1.0f, 1.0f });

    m_triangleData.push_back(center - upVectorPlane - rightVectorPlane);
    m_triangleData.push_back(center - upVectorPlane + rightVectorPlane);
    m_triangleData.push_back(center + upVectorPlane + rightVectorPlane);

    m_triangleData.push_back(center - upVectorPlane - rightVectorPlane);
    m_triangleData.push_back(center + upVectorPlane + rightVectorPlane);
    m_triangleData.push_back(center + upVectorPlane - rightVectorPlane);

    m_triangleColor.push_back(color);
    m_triangleColor.push_back(color);
    m_triangleColor.push_back(color);

    m_triangleColor.push_back(color);
    m_triangleColor.push_back(color);
    m_triangleColor.push_back(color);
}

void DebugDraw::DrawCircle(const glm::vec3& center, float radius, const Common::Math::Plane& plane, const glm::vec4& color)
{

}

void DebugDraw::DrawLine(const Common::Math::Line& line, float length, const glm::vec4& color)
{
    glm::vec3 center = Common::Math::GetClosestPointOnLine(line, { 0.0f, 0.0f, 0.0f });
    glm::vec3 vector = glm::normalize(line.point - center);

    DrawLine(center - vector * (length / 2.0f), center + vector * (length / 2.0f), color);
}

void DebugDraw::DrawPlane(const Common::Math::Plane& plane, const glm::vec2& halfExtents, const glm::vec4& color)
{
    // compute the center of plane
    // it will be on some axis (plane must intercept at least one axis)
    glm::vec3 center;

    // if normal component is not zero there is interception of plane with that axis
    // compute value of that component such other components are set to zero
    if (plane.normal.y != 0.0f)
        center = glm::vec3(0.0f, -plane.value / plane.normal.y, 0.0f);
    else if (plane.normal.x != 0.0f)
        center = glm::vec3(-plane.value / plane.normal.x, 0.0f, 0.0f);
    else
        center = glm::vec3(0.0f, 0.0f, -plane.value / plane.normal.z);

    DrawRectangle(center, halfExtents, plane.normal, color);
}

void DebugDraw::Draw(const glm::mat4& view, const glm::mat4& projection)
{
    glDisable(GL_DEPTH_TEST);
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
#if !defined(ANDROID)
    glEnable(GL_PROGRAM_POINT_SIZE);
#endif

    DrawCommon(view, projection, GL_POINTS, m_pointData, m_pointColor);

#if !defined(ANDROID)
    glDisable(GL_PROGRAM_POINT_SIZE);
#endif
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
    m_pointData.clear();
    m_pointColor.clear();

    m_lineData.clear();
    m_lineColor.clear();

    m_triangleData.clear();
    m_triangleColor.clear();
}
