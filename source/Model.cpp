#include "Model.h"
#include "OpenGL.h"
#include "model_generated.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Texture.h"
#include "Shader.h"
#include "Common.h"

Mesh::Mesh(const ModelData::MeshT & mesh, const std::string & root, uint32_t flags)
        : m_flags(flags)
{
    printf("Mesh constructor\n");

    InitBuffers(mesh.positions, mesh.normals, mesh.texCoords, mesh.tangents, mesh.bitangents, mesh.indices);
    if (!InitTextures(mesh.textures, root))
    {
        throw std::runtime_error("Error initializing textures.");
    }
}

Mesh::~Mesh()
{
    glDeleteBuffers(1, &m_bufferPositions);
    glDeleteBuffers(1, &m_bufferIndices);

    if (m_flags & Model::FlagNormal)
        glDeleteBuffers(1, &m_bufferNormals);

    if (m_flags & Model::FlagTextureDiffuse || m_flags & Model::FlagTextureNormal || m_flags & Model::FlagTextureSpecular)
        glDeleteBuffers(1, &m_bufferTexCoords);

    if (m_flags & Model::FlagTextureNormal)
    {
        glDeleteBuffers(1, &m_bufferTangents);
        glDeleteBuffers(1, &m_bufferBitangents);
    }

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
    shader.Begin();

    if (m_flags & Model::FlagTextureDiffuse)
        BindTextures(m_textureDiffuse, "textureDiffuse", shader);

    if (m_flags & Model::FlagTextureNormal)
        BindTextures(m_textureNormal, "textureNormal", shader);

    if (m_flags & Model::FlagTextureSpecular)
        BindTextures(m_textureSpecular, "textureSpecular", shader);
    //BindTextures(m_textureAmbient, "textureAmbient", shader);
    //BindTextures(m_textureHeight, "textureHeight", shader);

    shader.BindBuffer<glm::vec3>(m_bufferPositions, "positionModelSpace");

    if (m_flags & Model::FlagNormal)
        shader.BindBuffer<glm::vec3>(m_bufferNormals, "normalModelSpace");

    if (m_flags & Model::FlagTextureDiffuse || m_flags & Model::FlagTextureNormal)
        shader.BindBuffer<glm::vec2>(m_bufferTexCoords, "vertexUV");

    if (m_flags & Model::FlagTextureNormal)
    {
        shader.BindBuffer<glm::vec3>(m_bufferTangents, "tangentModelSpace");
        shader.BindBuffer<glm::vec3>(m_bufferBitangents, "bitangentModelSpace");
    }

    shader.BindElementBuffer(m_bufferIndices);

    glDrawElements(GL_TRIANGLES,      // mode
        m_verticesCount,   // count
        GL_UNSIGNED_SHORT, // type
        (void*)0);         // element array buffer offset

    if (GLenum error = glGetError(); error != GL_NO_ERROR)
    {
        printf("OpenGl error calling function glDrawElements (%d)", error);
        throw std::runtime_error("OpenGl error");
    }

    // TODO end here???
    shader.End();
    //glBindVertexArray(0);
    //// always good practice to set everything back to defaults once configured.
    //glActiveTexture(GL_TEXTURE0);
}
 
void Mesh::InitBuffers(const std::vector<ModelData::Vec3> & positions,
    const std::vector<ModelData::Vec3> & normals,
    const std::vector<ModelData::Vec2> & texCoords,
    const std::vector<ModelData::Vec3> & tangents,
    const std::vector<ModelData::Vec3> & bitangents,
    const std::vector<uint16_t> & indices)
{
    glGenBuffers(1, &m_bufferPositions);
    glBindBuffer(GL_ARRAY_BUFFER, m_bufferPositions);
    glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec3), &positions[0], GL_STATIC_DRAW);

    if (m_flags & Model::FlagTextureDiffuse)
    {
        glGenBuffers(1, &m_bufferTexCoords);
        glBindBuffer(GL_ARRAY_BUFFER, m_bufferTexCoords);
        glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(glm::vec2), &texCoords[0], GL_STATIC_DRAW);
    }

    if (m_flags & Model::FlagNormal)
    {
        glGenBuffers(1, &m_bufferNormals);
        glBindBuffer(GL_ARRAY_BUFFER, m_bufferNormals);
        glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
    }

    if (m_flags & Model::FlagTextureNormal)
    {
        glGenBuffers(1, &m_bufferTangents);
        glBindBuffer(GL_ARRAY_BUFFER, m_bufferTangents);
        glBufferData(GL_ARRAY_BUFFER, tangents.size() * sizeof(glm::vec3), &tangents[0], GL_STATIC_DRAW);

        glGenBuffers(1, &m_bufferBitangents);
        glBindBuffer(GL_ARRAY_BUFFER, m_bufferBitangents);
        glBufferData(GL_ARRAY_BUFFER, bitangents.size() * sizeof(glm::vec3), &bitangents[0], GL_STATIC_DRAW);
    }

    glGenBuffers(1, &m_bufferIndices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufferIndices);
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

Model::Model(const char * path, uint32_t flags)
{
    auto data = Common::ReadFile(path);
    auto model = ModelData::UnPackModel(data.data());
    auto root = Common::GetDirectoryFromFilePath(path);

    for (size_t i = 0; i < model->meshes.size(); ++i)
    {
        // TODO check is mesh constructed successfully
        m_meshes.push_back(std::make_unique<Mesh>(*(model->meshes[i].get()), root, flags));
    }
}

void Model::Draw(Shader & shader)
{
    for (auto & mesh : m_meshes)
        mesh->Draw(shader);
}
