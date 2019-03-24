#include "Scene.h"

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

Shape::Cube::Cube(ModelShader & ashader)
    : shader(ashader)
{
    if (IsVAOSupported())
    {
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
    }

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VERTICES_CUBE), VERTICES_CUBE, GL_STATIC_DRAW);
    CheckGlError("glBufferData");

    if (IsVAOSupported())
    {
        glEnableVertexAttribArray(shader.GetLocations().buffers.positions);
        glVertexAttribPointer(shader.GetLocations().buffers.positions, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);

        glEnableVertexAttribArray(shader.GetLocations().buffers.normals);
        glVertexAttribPointer(shader.GetLocations().buffers.normals, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));

        //glEnableVertexAttribArray(m_locationVertexUV);
        //glVertexAttribPointer(m_locationVertexUV, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

        CheckGlError("glEnableVertexAttribArray");

        glBindVertexArray(0);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Shape::Cube::~Cube()
{
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
}

void Shape::Cube::Bind()
{
    if (IsVAOSupported())
    {
        shader.GetShader().BindVAO(vao);
    }
    else
    {
        // TODO this should be bound using shader

        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        glEnableVertexAttribArray(shader.GetLocations().buffers.positions);
        glVertexAttribPointer(shader.GetLocations().buffers.positions, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);

        glEnableVertexAttribArray(shader.GetLocations().buffers.normals);
        glVertexAttribPointer(shader.GetLocations().buffers.normals, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));

        //glEnableVertexAttribArray(m_locationVertexUV);
        //glVertexAttribPointer(m_locationVertexUV, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    }
}

void Shape::Cube::Draw()
{
    glDrawArrays(GL_TRIANGLES, 0, 36);
    CheckGlError("glDrawArrays");
}

Scene::Scene(const Light::Config & light)
{
    ModelShader::Config config;

    config.light = light;
    config.shading = ModelShader::ShadingModel::BlinnPhong;
    config.flags = (uint32_t)ModelShader::Config::Flags::UseRuntimeMaterial;

    m_shader = std::make_unique<ModelShader>(config);
    m_cube = std::make_unique<Shape::Cube>(*m_shader.get());
}

void Scene::AddCube(const glm::mat4 & model, const Material::Data & material)
{
    m_cubes.push_back({ model, material });
}

void Scene::Draw(const glm::mat4 & view, const glm::mat4 & projection, const glm::vec3 & cameraPosition, const Light::Data & data)
{
    m_shader->BeginRender();

    while (m_shader->BeginRenderShadow(data))
    {
        m_cube->Bind();

        for (const auto &[model, _] : m_cubes)
        {
            m_shader->BindTransformShadow(model);
            m_cube->Draw();
        }

        m_shader->EndRenderShadow();
    }

    m_shader->BindCamera(cameraPosition);
    m_shader->BindLight(data);

    m_cube->Bind();

    for (const auto & [model, material] : m_cubes)
    {
        m_shader->BindMaterial(material);
        m_shader->BindTransform(model, view, projection);

        m_cube->Draw();
    }

    m_shader->EndRender();
}
