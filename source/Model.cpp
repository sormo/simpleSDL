#include "Model.h"
#include "OpenGL.h"
#include "model_generated.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Texture.h"
#include "Shader.h"
#include "Common.h"

Mesh::Mesh(const ModelData::MeshT & mesh, const std::string & root, Shader & shader, uint32_t flags)
        : m_flags(flags)
{
    InitBuffers(mesh.positions, mesh.normals, mesh.texCoords, mesh.tangents, mesh.bitangents, mesh.indices, shader);
    if (!InitTextures(mesh.textures, root))
    {
        throw std::runtime_error("Error initializing textures.");
    }
}

Mesh::~Mesh()
{
    glDeleteBuffers(1, &m_positions.vbo);
    glDeleteBuffers(1, &m_vboIndices);

    if (m_flags & Model::FlagNormal)
        glDeleteBuffers(1, &m_normals.vbo);

    if (m_flags & Model::FlagTextureDiffuse || m_flags & Model::FlagTextureNormal || m_flags & Model::FlagTextureSpecular)
        glDeleteBuffers(1, &m_texCoords.vbo);

    if (m_flags & Model::FlagTextureNormal)
    {
        glDeleteBuffers(1, &m_tangents.vbo);
        glDeleteBuffers(1, &m_bitangents.vbo);
    }

    if (m_flags & Model::FlagVAO)
        glDeleteVertexArrays(1, &m_vao);

    glDeleteTextures(m_textureDiffuse.size(), m_textureDiffuse.data());
    glDeleteTextures(m_textureNormal.size(), m_textureNormal.data());
    glDeleteTextures(m_textureSpecular.size(), m_textureSpecular.data());
    glDeleteTextures(m_textureAmbient.size(), m_textureAmbient.data());
    glDeleteTextures(m_textureHeight.size(), m_textureHeight.data());
}

void Mesh::BindTextures(const std::vector<GLuint> & textures, const char * name, Shader & shader)
{
    for (uint32_t i = 0; i < textures.size(); i++)
    {
        std::string location(name);
        location += std::to_string(i);

        shader.BindTexture(textures[i], location.c_str());
    }
}

// render the mesh
void Mesh::Draw(Shader & shader)
{
    if (m_flags & Model::FlagTextureDiffuse)
        BindTextures(m_textureDiffuse, "textureDiffuse", shader);

    if (m_flags & Model::FlagTextureNormal)
        BindTextures(m_textureNormal, "textureNormal", shader);

    if (m_flags & Model::FlagTextureSpecular)
        BindTextures(m_textureSpecular, "textureSpecular", shader);
    //BindTextures(m_textureAmbient, "textureAmbient", shader);
    //BindTextures(m_textureHeight, "textureHeight", shader);

    if (m_flags & Model::FlagVAO)
    {
        shader.BindVAO(m_vao);
    }
    else
    {
        shader.BindBuffer<glm::vec3>(m_positions.vbo, m_positions.index);

        if (m_flags & Model::FlagNormal)
            shader.BindBuffer<glm::vec3>(m_normals.vbo, m_normals.index);

        if (m_flags & Model::FlagTextureDiffuse || m_flags & Model::FlagTextureNormal)
            shader.BindBuffer<glm::vec2>(m_texCoords.vbo, m_texCoords.index);

        if (m_flags & Model::FlagTextureNormal)
        {
            shader.BindBuffer<glm::vec3>(m_tangents.vbo, m_tangents.index);
            shader.BindBuffer<glm::vec3>(m_bitangents.vbo, m_bitangents.index);
        }
    }

    // TODO can be bound to VAO ???
    shader.BindElementBuffer(m_vboIndices);

    glDrawElements(GL_TRIANGLES, m_verticesCount, GL_UNSIGNED_SHORT, (void*)0);
    CheckGlError("glDrawElements");

    shader.CleanUp();
}

template<class T, uint32_t N>
Mesh::VBO CreateFloatVBO(GLuint index, const std::vector<T> & data, bool isVaoBinded)
{
    GLuint vbo;

    glGenBuffers(1, &vbo);
    CheckGlError("glGenBuffers");

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    CheckGlError("glBindBuffer");

    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(T), &data[0], GL_STATIC_DRAW);
    CheckGlError("glBufferData");

    if (isVaoBinded)
    {
        glEnableVertexAttribArray(index);
        CheckGlError("glEnableVertexAttribArray");

        glVertexAttribPointer(index, N, GL_FLOAT, GL_FALSE, 0, nullptr);
        CheckGlError("glVertexAttribPointer");
    }

    return { vbo, index };
}

void Mesh::InitBuffers(const std::vector<ModelData::Vec3> & positions,
    const std::vector<ModelData::Vec3> & normals,
    const std::vector<ModelData::Vec2> & texCoords,
    const std::vector<ModelData::Vec3> & tangents,
    const std::vector<ModelData::Vec3> & bitangents,
    const std::vector<uint16_t> & indices,
    Shader & shader)
{
    bool bindVAO = m_flags & Model::FlagVAO;

    // prepare and bind VAO if possible
    if (bindVAO)
    {
        glGenVertexArrays(1, &m_vao);
        CheckGlError("glGenVertexArrays");

        glBindVertexArray(m_vao);
        CheckGlError("glBindVertexArray");
    }

    m_positions = CreateFloatVBO<ModelData::Vec3, 3>(shader.GetLocation("positionModelSpace", Shader::LocationType::Attrib), positions, bindVAO);

    if (m_flags & Model::FlagNormal)
        m_normals = CreateFloatVBO<ModelData::Vec3, 3>(shader.GetLocation("normalModelSpace", Shader::LocationType::Attrib), normals, bindVAO);

    if (m_flags & Model::FlagTextureDiffuse)
    {
        m_texCoords = CreateFloatVBO<ModelData::Vec2, 2>(shader.GetLocation("vertexUV", Shader::LocationType::Attrib), texCoords, bindVAO);
    }

    if (m_flags & Model::FlagTextureNormal)
    {
        m_tangents = CreateFloatVBO<ModelData::Vec3, 3>(shader.GetLocation("tangentModelSpace", Shader::LocationType::Attrib), tangents, bindVAO);
        m_bitangents = CreateFloatVBO<ModelData::Vec3, 3>(shader.GetLocation("bitangentModelSpace", Shader::LocationType::Attrib), bitangents, bindVAO);
    }

    glBindVertexArray(0);

    // TODO can be element buffer binded to vao ???
    glGenBuffers(1, &m_vboIndices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboIndices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint16_t), &indices[0], GL_STATIC_DRAW);

    m_verticesCount = indices.size();
}

std::vector<GLuint> & Mesh::GetTextureContainer(ModelData::TextureType type)
{
    switch (type)
    {
    case ModelData::TextureType_Diffuse:
        return m_textureDiffuse;
    case ModelData::TextureType_Specular:
        return m_textureSpecular;
    case ModelData::TextureType_Normal:
        return m_textureNormal;
    case ModelData::TextureType_Ambient:
        return m_textureAmbient;
    case ModelData::TextureType_Height:
        return m_textureHeight;
    }

    assert(false);
    return m_textureDiffuse;
}

bool Mesh::InitTextures(const std::vector<std::unique_ptr<ModelData::TextureT>> & textures, const std::string & root)
{
    for (const auto & texturePtr : textures)
    {
        auto & container = GetTextureContainer(texturePtr->type);
        auto newTexture = Texture::Load((root + texturePtr->path).c_str());
        if (!newTexture)
        {
            printf("Error loading texture %s.\n", texturePtr->path.c_str());
            return false;
        }
        container.push_back(*newTexture);
    }
    return true;
}

Model::Model(const char * path, uint32_t flags, ShaderPtr & shader)
    : m_shader(shader), m_flags(flags)
{
    auto data = Common::ReadFile(path);
    auto model = ModelData::UnPackModel(data.data());
    auto root = Common::GetDirectoryFromFilePath(path);

    if (IsVAOSupported())
        flags |= FlagVAO;

    for (size_t i = 0; i < model->meshes.size(); ++i)
    {
        // TODO check is mesh constructed successfully
        m_meshes.push_back(std::make_unique<Mesh>(*(model->meshes[i].get()), root, *m_shader, flags));
    }
}

void Model::Bind(const glm::vec3 & cameraPosition, const Light & light)
{
    m_shader->Begin();

    m_shader->SetUniform(cameraPosition, "cameraWorldSpace");
    m_shader->SetUniform(light.position, "light.position");
    m_shader->SetUniform(light.ambient, "light.ambient");
    m_shader->SetUniform(light.diffuse, "light.diffuse");
    m_shader->SetUniform(light.specular, "light.specular");

    m_shader->SetUniform(10.0f, "shininess");
}

void Model::Bind(const glm::vec3 & cameraPosition, const LightDirectional & light)
{
    m_shader->Begin();

    m_shader->SetUniform(cameraPosition, "cameraWorldSpace");
    m_shader->SetUniform(light.direction, "light.direction");
    m_shader->SetUniform(light.ambient, "light.ambient");
    m_shader->SetUniform(light.diffuse, "light.diffuse");
    m_shader->SetUniform(light.specular, "light.specular");

    m_shader->SetUniform(10.0f, "shininess");
}

void Model::Bind(const glm::vec3 & cameraPosition, const Light & light, const Material & material)
{
    m_shader->Begin();

    assert(m_flags & FlagMaterial);

    m_shader->SetUniform(material.ambient, "material.ambient");
    m_shader->SetUniform(material.diffuse, "material.diffuse");
    m_shader->SetUniform(material.specular, "material.specular");
    m_shader->SetUniform(material.shininess, "material.shininess");

    Bind(cameraPosition, light);
}

void Model::Draw(const glm::mat4 & model, const glm::mat4 & view, const glm::mat4 & projection)
{
    glm::mat4 MVP = projection * view * model;
    m_shader->SetUniform(MVP, "MVP");
    m_shader->SetUniform(model, "M");

    for (auto & mesh : m_meshes)
        mesh->Draw(*m_shader);
}
