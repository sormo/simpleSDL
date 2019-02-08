#include "Model.h"
#include "OpenGL.h"
#include "model_generated.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Texture.h"
#include "Shader.h"
#include "Common.h"

Mesh::Mesh(const ModelData::MeshT & mesh, const std::string & root, ModelShader & shader)
    : m_shader(&shader)
{
    InitBuffers(mesh.positions, mesh.normals, mesh.texCoords, mesh.tangents, mesh.bitangents, mesh.indices);

    if (!InitTextures(mesh.textures, root))
    {
        throw std::runtime_error("Error initializing textures.");
    }
}

Mesh::~Mesh()
{
    glDeleteBuffers(1, &m_positions);
    glDeleteBuffers(1, &m_vboIndices);
    glDeleteBuffers(1, &m_normals);

    if (m_shader->GetConfig().NeedsUVs())
        glDeleteBuffers(1, &m_texCoords);

    if (m_shader->GetConfig().textureNormalCount)
    {
        glDeleteBuffers(1, &m_tangents);
        glDeleteBuffers(1, &m_bitangents);
    }

    if (IsVAOSupported())
        glDeleteVertexArrays(1, &m_vao);

    glDeleteTextures(m_textureDiffuse.size(), m_textureDiffuse.data());
    glDeleteTextures(m_textureNormal.size(), m_textureNormal.data());
    glDeleteTextures(m_textureSpecular.size(), m_textureSpecular.data());
    glDeleteTextures(m_textureAmbient.size(), m_textureAmbient.data());
    glDeleteTextures(m_textureHeight.size(), m_textureHeight.data());
}

// render the mesh
void Mesh::Draw()
{
    // bind mesh specific data

    for (size_t i = 0; i < m_shader->GetConfig().textureAmbientCount; ++i)
        m_shader->GetShader().BindTexture(m_textureAmbient[i], m_shader->GetLocations().textureAmbient[i]);

    for (size_t i = 0; i < m_shader->GetConfig().textureDiffuseCount; ++i)
        m_shader->GetShader().BindTexture(m_textureDiffuse[i], m_shader->GetLocations().textureDiffuse[i]);

    for (size_t i = 0; i < m_shader->GetConfig().textureSpecularCount; ++i)
        m_shader->GetShader().BindTexture(m_textureSpecular[i], m_shader->GetLocations().textureSpecular[i]);

    for (size_t i = 0; i < m_shader->GetConfig().textureNormalCount; ++i)
        m_shader->GetShader().BindTexture(m_textureNormal[i], m_shader->GetLocations().textureNormal[i]);

    //BindTextures(m_textureHeight, "textureHeight", shader);

    if (IsVAOSupported())
    {
        m_shader->GetShader().BindVAO(m_vao);
    }
    else
    {
        m_shader->GetShader().BindBuffer<glm::vec3>(m_positions, m_shader->GetLocations().positions);
        m_shader->GetShader().BindBuffer<glm::vec3>(m_normals, m_shader->GetLocations().normals);

        if (m_shader->GetConfig().NeedsUVs())
            m_shader->GetShader().BindBuffer<glm::vec2>(m_texCoords, m_shader->GetLocations().texCoords);

        if (m_shader->GetConfig().textureNormalCount)
        {
            m_shader->GetShader().BindBuffer<glm::vec3>(m_tangents, m_shader->GetLocations().tangents);
            //m_shader->GetShader().BindBuffer<glm::vec3>(m_bitangents, m_shader->GetLocations().bitangents);
        }
    }

    // TODO can be bound to VAO ???
    m_shader->GetShader().BindElementBuffer(m_vboIndices);

    glDrawElements(GL_TRIANGLES, m_verticesCount, GL_UNSIGNED_SHORT, (void*)0);
    CheckGlError("glDrawElements");

    m_shader->GetShader().CleanUp();
}

template<class T, uint32_t N>
GLuint CreateFloatVBO(GLuint index, const std::vector<T> & data, bool isVaoBinded)
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

    return vbo;
}

void Mesh::InitBuffers(const std::vector<ModelData::Vec3> & positions,
                        const std::vector<ModelData::Vec3> & normals,
                        const std::vector<ModelData::Vec2> & texCoords,
                        const std::vector<ModelData::Vec3> & tangents,
                        const std::vector<ModelData::Vec3> & bitangents,
                        const std::vector<uint16_t> & indices)
{
    bool bindVAO = IsVAOSupported();

    // prepare and bind VAO if possible
    if (bindVAO)
    {
        glGenVertexArrays(1, &m_vao);
        CheckGlError("glGenVertexArrays");

        glBindVertexArray(m_vao);
        CheckGlError("glBindVertexArray");
    }

    m_positions = CreateFloatVBO<ModelData::Vec3, 3>(m_shader->GetShader().GetLocation("positionModelSpace", Shader::LocationType::Attrib), positions, bindVAO);
    m_normals = CreateFloatVBO<ModelData::Vec3, 3>(m_shader->GetShader().GetLocation("normalModelSpace", Shader::LocationType::Attrib), normals, bindVAO);

    if (m_shader->GetConfig().textureNormalCount)
    {
        m_tangents = CreateFloatVBO<ModelData::Vec3, 3>(m_shader->GetShader().GetLocation("tangentModelSpace", Shader::LocationType::Attrib), tangents, bindVAO);
        //m_bitangents = CreateFloatVBO<ModelData::Vec3, 3>(m_shader->GetShader().GetLocation("bitangentModelSpace", Shader::LocationType::Attrib), bitangents, bindVAO);
    }

    if (m_shader->GetConfig().NeedsUVs())
    {
        m_texCoords = CreateFloatVBO<ModelData::Vec2, 2>(m_shader->GetShader().GetLocation("vertexUV", Shader::LocationType::Attrib), texCoords, bindVAO);
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

Model::Model(const char * path, ModelShaderPtr & shader)
    : m_shader(shader)
{
    auto data = Common::ReadFile(path);
    auto model = ModelData::UnPackModel(data.data());
    auto root = Common::GetDirectoryFromFilePath(path);

    for (size_t i = 0; i < model->meshes.size(); ++i)
    {
        // TODO check is mesh constructed successfully
        m_meshes.push_back(std::make_unique<Mesh>(*(model->meshes[i].get()), root, *m_shader));
    }
}

void Model::Bind(const ModelShader::Data & data)
{
    m_shader->Bind(data);
}

void Model::Draw(const glm::mat4 & model, const glm::mat4 & view, const glm::mat4 & projection)
{
    glm::mat4 MVP = projection * view * model;
    m_shader->GetShader().SetUniform(MVP, "MVP");
    m_shader->GetShader().SetUniform(model, "M");

    for (auto & mesh : m_meshes)
        mesh->Draw();
}
