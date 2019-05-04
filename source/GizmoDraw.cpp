#include "GizmoDraw.h"

static const char VERTEX_SHADER[] = \
"#version 100\n"
"#ifdef GL_ES\n"
"precision mediump float;\n"
"#endif\n"
"attribute vec3 positionModelSpace;\n"
"uniform mat4 MVP;\n"
"void main(void)\n"
"{\n"
"    gl_Position =  MVP * vec4(positionModelSpace, 1.0);\n"
"}\n";

static const char FRAGMENT_SHADER[] = \
"#version 100\n"
"#ifdef GL_ES\n"
"precision mediump float;\n"
"#endif\n"
"uniform vec4 color;\n"
"void main(void)\n"
"{\n"
"    gl_FragColor = color;\n"
"}\n";

GizmoDraw::GizmoDraw()
{
    m_postprocess = std::make_unique<Postprocess>(Postprocess::Type::NoChange);

    m_shader = std::make_unique<Shader>(VERTEX_SHADER, FRAGMENT_SHADER);

    // initialize shapes shader
    m_shapesShader.shader = m_shader.get();
    m_shapesShader.locationPosition = m_shader->GetLocation("positionModelSpace", Shader::LocationType::Attrib);

    m_sphere = std::make_unique<Shapes::Sphere>(&m_shapesShader);
    m_cylinder = std::make_unique<Shapes::Cylinder>(&m_shapesShader);
    m_cone = std::make_unique<Shapes::Cone>(&m_shapesShader);
    m_cube = std::make_unique<Shapes::Cube>(&m_shapesShader);
}

void GizmoDraw::AddShape(Scene::Shape shape, const glm::vec4& color)
{
    auto GetShape = [this](Scene::Shape shape)
    {
        switch (shape->GetType())
        {
        case Shapes::Type::Cone:
            return (Shapes::Shape*)m_cone.get();
        case Shapes::Type::Cylinder:
            return (Shapes::Shape*)m_cylinder.get();
        case Shapes::Type::Sphere:
            return (Shapes::Shape*)m_sphere.get();
        case Shapes::Type::Cube:
            return (Shapes::Shape*)m_cube.get();
        }
        return (Shapes::Shape*)nullptr;
    };

    m_shapes.insert({ GetShape(shape), {shape, color} });
}

void GizmoDraw::DrawShapes(const glm::mat4& view, const glm::mat4& projection)
{
    if (m_shapes.empty())
        return;

    //glDisable(GL_DEPTH_TEST);

    // enable blending
    glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    m_postprocess->BeginRender();

    //glViewport(0, 0, Common::GetWindowWidth(), Common::GetWindowHeight());
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_shader->BeginRender();

    auto it = m_shapes.begin();
    Shapes::Shape* shape = it->first;
    shape->Bind();

    while (it != std::end(m_shapes))
    {
        if (it->first != shape)
        {
            shape = it->first;
            shape->Bind();
        }

        while (it != std::end(m_shapes) && it->first == shape)
        {
            auto&[sceneShape, color] = it->second;

            glm::mat4 mvp = projection * view * sceneShape->GetTransform();
            m_shader->SetUniform(mvp, "MVP");
            m_shader->SetUniform(color, "color");

            shape->Draw();

            it++;
        }
    }

    m_shader->EndRender();

    m_postprocess->EndRender();

    glDisable(GL_BLEND);

    //glEnable(GL_DEPTH_TEST);
}

void GizmoDraw::ClearShapes()
{
    m_shapes.clear();
}
