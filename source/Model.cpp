#include "Model.h"
#include "OpenGL.h"
#include "model_generated.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Texture.h"
#include "Shader.h"
#include "Common.h"
#include "CommonProject.h"
#include "TextureManager.h"

Mesh::Mesh(const ModelData::MeshT & mesh, ModelMaterial & material)
    : m_material(material)
{
    InitBuffers(mesh.positions, mesh.normals, mesh.texCoords, mesh.tangents, mesh.bitangents, mesh.indices);
}

Mesh::~Mesh()
{
    glDeleteBuffers(1, &m_positions);
    glDeleteBuffers(1, &m_vboIndices);
    glDeleteBuffers(1, &m_normals);

    if (m_material.shader->GetConfig().GetUVChannelsCount())
        glDeleteBuffers(m_texCoords.size(), m_texCoords.data());

    if (m_material.shader->GetConfig().textures.normal.size())
    {
        glDeleteBuffers(1, &m_tangents);
        glDeleteBuffers(1, &m_bitangents);
    }

    if (IsVAOSupported())
        glDeleteVertexArrays(1, &m_vao);
}

// render the mesh
void Mesh::Draw()
{
    // bind mesh specific data

    for (size_t i = 0; i < m_material.shader->GetConfig().textures.ambient.size(); ++i)
        m_material.shader->GetShader().BindTexture(m_material.textures.ambient[i], m_material.shader->GetLocations().textureAmbient[i]);

    for (size_t i = 0; i < m_material.shader->GetConfig().textures.diffuse.size(); ++i)
        m_material.shader->GetShader().BindTexture(m_material.textures.diffuse[i], m_material.shader->GetLocations().textureDiffuse[i]);

    for (size_t i = 0; i < m_material.shader->GetConfig().textures.specular.size(); ++i)
        m_material.shader->GetShader().BindTexture(m_material.textures.specular[i], m_material.shader->GetLocations().textureSpecular[i]);

    for (size_t i = 0; i < m_material.shader->GetConfig().textures.normal.size(); ++i)
        m_material.shader->GetShader().BindTexture(m_material.textures.normal[i], m_material.shader->GetLocations().textureNormal[i]);

    //BindTextures(m_textureHeight, "textureHeight", shader);

    if (IsVAOSupported())
    {
        m_material.shader->GetShader().BindVAO(m_vao);
    }
    else
    {
        m_material.shader->GetShader().BindBuffer<glm::vec3>(m_positions, m_material.shader->GetLocations().positions);
        m_material.shader->GetShader().BindBuffer<glm::vec3>(m_normals, m_material.shader->GetLocations().normals);

        for (uint32_t i = 0; i < m_material.shader->GetConfig().GetUVChannelsCount(); ++i)
            m_material.shader->GetShader().BindBuffer<glm::vec2>(m_texCoords[i], m_material.shader->GetLocations().texCoords[i]);

        if (m_material.shader->GetConfig().textures.normal.size())
        {
            m_material.shader->GetShader().BindBuffer<glm::vec3>(m_tangents, m_material.shader->GetLocations().tangents);
            //m_material.shader->GetShader().BindBuffer<glm::vec3>(m_bitangents, m_shader->GetLocations().bitangents);
        }
    }

    // TODO can be bound to VAO ???
    m_material.shader->GetShader().BindElementBuffer(m_vboIndices);

    glDrawElements(GL_TRIANGLES, m_verticesCount, GL_UNSIGNED_SHORT, (void*)0);
    CheckGlError("glDrawElements");

    m_material.shader->GetShader().CleanUp();
}

template<class T, uint32_t N>
GLuint CreateFloatVBO(GLuint index, const T * data, size_t size, bool isVaoBinded)
{
    GLuint vbo;

    glGenBuffers(1, &vbo);
    CheckGlError("glGenBuffers");

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    CheckGlError("glBindBuffer");

    glBufferData(GL_ARRAY_BUFFER, size*sizeof(T), data, GL_STATIC_DRAW);
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

template<class T, uint32_t N>
GLuint CreateFloatVBO(GLuint index, const std::vector<T> & data, bool isVaoBinded)
{
    return CreateFloatVBO<T, N>(index, &data[0], data.size(), isVaoBinded);
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

    m_positions = CreateFloatVBO<ModelData::Vec3, 3>(m_material.shader->GetShader().GetLocation("positionModelSpace", Shader::LocationType::Attrib), positions, bindVAO);
    m_normals = CreateFloatVBO<ModelData::Vec3, 3>(m_material.shader->GetShader().GetLocation("normalModelSpace", Shader::LocationType::Attrib), normals, bindVAO);

    if (m_material.shader->GetConfig().textures.normal.size())
    {
        m_tangents = CreateFloatVBO<ModelData::Vec3, 3>(m_material.shader->GetShader().GetLocation("tangentModelSpace", Shader::LocationType::Attrib), tangents, bindVAO);
        //m_bitangents = CreateFloatVBO<ModelData::Vec3, 3>(m_shader->GetShader().GetLocation("bitangentModelSpace", Shader::LocationType::Attrib), bitangents, bindVAO);
    }

    for (uint32_t i = 0; i < m_material.shader->GetConfig().GetUVChannelsCount(); ++i)
    {
        m_texCoords.push_back(CreateFloatVBO<ModelData::Vec2, 2>(m_material.shader->GetShader().GetLocation("vertexUV" + std::to_string(i), Shader::LocationType::Attrib),
            &texCoords[i * positions.size()], positions.size(), bindVAO));
    }

    glBindVertexArray(0);

    // TODO can be element buffer binded to vao ???
    glGenBuffers(1, &m_vboIndices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboIndices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint16_t), &indices[0], GL_STATIC_DRAW);

    m_verticesCount = indices.size();
}

ModelShader::TextureStackEntry::Operation Convert(ModelData::TextureOperation operation)
{
    switch (operation)
    {
    case ModelData::TextureOperation_Add:
        return ModelShader::TextureStackEntry::Operation::Add;
    case ModelData::TextureOperation_Multiply:
        return ModelShader::TextureStackEntry::Operation::Multiply;
    case ModelData::TextureOperation_Substract:
        return ModelShader::TextureStackEntry::Operation::Substract;
    case ModelData::TextureOperation_Divide:
        return ModelShader::TextureStackEntry::Operation::Divide;
    case ModelData::TextureOperation_SmoothAdd:
        return ModelShader::TextureStackEntry::Operation::SmoothAdd;
    case ModelData::TextureOperation_SignedAdd:
        return ModelShader::TextureStackEntry::Operation::SignedAdd;
    }
    return ModelShader::TextureStackEntry::Operation::Add;
}

bool ProcessTextures(const std::string & root, std::vector<std::unique_ptr<ModelData::TextureT>> & data, std::vector<ModelShader::TextureStackEntry> & stack, std::vector<GLuint> & textures)
{
    for (size_t i = 0; i < data.size(); ++i)
    {
        ModelShader::TextureStackEntry entry;
        entry.factor = data[i]->blendFactor;
        entry.operation = Convert(data[i]->operation);
        entry.uvIndex = data[i]->uvIndex;

        auto texture = TextureManager::Instance().GetTexture((root + data[i]->path).c_str());
        if (!texture)
        {
            printf("Error loading texture %s", (root + data[i]->path).c_str());
            return false;
        }

        textures.push_back(*texture);
        stack.push_back(entry);
    }

    return true;
}

std::unique_ptr<ModelMaterial> Model::CreateMaterial(const std::string & root, ModelData::MaterialT & material)
{
    ModelShader::Config config;
    std::unique_ptr<ModelMaterial> result = std::make_unique<ModelMaterial>();

    config.light = m_configLight;

    memset(&config.material, 0, sizeof(config.material));

    if (material.ambient)
        config.material.ambient = Convert(*material.ambient);
    if (material.diffuse)
        config.material.diffuse = Convert(*material.diffuse);
    if (material.specular)
        config.material.specular = Convert(*material.specular);
    config.material.shininess = material.shininess;
    config.material.shininessStrength = material.shininessStrength;

    if (!ProcessTextures(root, material.textureAmbient, config.textures.ambient, result->textures.ambient))
        return nullptr;
    if (!ProcessTextures(root, material.textureDiffuse, config.textures.diffuse, result->textures.diffuse))
        return nullptr;
    if (!ProcessTextures(root, material.textureSpecular, config.textures.specular, result->textures.specular))
        return nullptr;
    if (!ProcessTextures(root, material.textureNormal, config.textures.normal, result->textures.normal))
        return nullptr;

    result->shader = std::make_unique<ModelShader>(config);
    if (!result->shader.get())
        return nullptr;

    return result;
}

Model::Model(const char * path, ModelShader::ConfigLight light)
    : m_configLight(light)
{
    auto data = Common::ReadFile(path);
    auto model = ModelData::UnPackModel(data.data());
    auto root = Common::GetDirectoryFromFilePath(path);

    for (size_t i = 0; i < model->materials.size(); ++i)
    {
        auto material = CreateMaterial(root, *model->materials[i].get());
        if (!material)
        {
            printf("Error creating material.");
            throw std::runtime_error("Error creating material.");
        }
        
        m_materials.push_back(std::move(material));
    }

    for (size_t i = 0; i < model->meshes.size(); ++i)
    {
        uint32_t materialIndex = model->meshes[i]->material;
        if (materialIndex >= m_materials.size())
        {
            printf("Error loading model, mesh has invalid material index (%d)", materialIndex);
            throw std::runtime_error("Error loading model.");
        }

        auto & material = *m_materials[materialIndex];

        // TODO check is mesh constructed successfully
        m_meshes.push_back(std::make_unique<Mesh>(*model->meshes[i].get(), material));
    }
}

Model::~Model()
{
    // clear meshes before materials
    m_meshes.clear();
}

void Model::Bind(const ModelShader::Data & data)
{
    for (auto & material : m_materials)
        material->shader->Bind(data);
}

void Model::Draw(const glm::mat4 & model, const glm::mat4 & view, const glm::mat4 & projection)
{
    glm::mat4 MVP = projection * view * model;

    for (auto & material : m_materials)
    {
        material->shader->GetShader().SetUniform(MVP, "MVP");
        material->shader->GetShader().SetUniform(model, "M");
    }

    for (auto & mesh : m_meshes)
        mesh->Draw();
}
