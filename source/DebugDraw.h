#pragma once
#include "Shader.h"
#include "glm/glm.hpp"
#include <vector>
#include "Common.h"

class DebugDraw
{
public:
    DebugDraw();

    void DrawPoint(const glm::vec3& center, const glm::vec4& color);
    void DrawLine(const glm::vec3& from, const glm::vec3& to, const glm::vec4& color);
    void DrawTriangle(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, const glm::vec4& color);
    
    void DrawSquare(const glm::vec3& p1, const glm::vec3& p2, const glm::vec4& color);

    void DrawPlane(const Common::Math::Plane& plane, const glm::vec4& color);
    void DrawLine(const Common::Math::Line& line, const glm::vec4& color);

    void DrawCircle(const glm::vec3& center, float radius, const Common::Math::Plane& plane, const glm::vec4& color);

    void Draw(const glm::mat4& view, const glm::mat4& projection);
    void Clear();

private:
    void DrawPoints(const glm::mat4& view, const glm::mat4& projection);
    void DrawLines(const glm::mat4& view, const glm::mat4& projection);
    void DrawTriangles(const glm::mat4& view, const glm::mat4& projection);
    void DrawCommon(const glm::mat4& view, const glm::mat4& projection,
        GLenum primitives, const std::vector<glm::vec3> & data, const std::vector<glm::vec4> & colors);

    Shader m_shader;
    GLuint m_locationVP;
    GLuint m_locationPosition;
    GLuint m_locationColor;

    std::vector<glm::vec3> m_pointData;
    std::vector<glm::vec4> m_pointColor;

    std::vector<glm::vec3> m_lineData;
    std::vector<glm::vec4> m_lineColor;

    std::vector<glm::vec3> m_triangleData;
    std::vector<glm::vec4> m_triangleColor;
};
