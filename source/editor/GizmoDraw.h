#pragma once
#include "utils/Shader.h"
#include "Shapes.h"
#include <set>
#include "scene/Scene.h"
#include "utils/Postprocess.h"

class GizmoDraw
{
public:
    GizmoDraw();

    void DrawShapes(const glm::mat4& view, const glm::mat4& projection);
    void AddShape(Scene::Shape shape, const glm::vec4& color);
    void ClearShapes();

private:
    std::unique_ptr<Postprocess> m_postprocess;
    std::unique_ptr<Shader> m_shader;

    Shapes::FreeShader m_shapesShader;
    std::unique_ptr<Shapes::Sphere> m_sphere;
    std::unique_ptr<Shapes::Cylinder> m_cylinder;
    std::unique_ptr<Shapes::Cone> m_cone;
    std::unique_ptr<Shapes::Cube> m_cube;

    std::multimap<Shapes::Shape*, std::tuple<Scene::Shape, glm::vec4>> m_shapes;
};
