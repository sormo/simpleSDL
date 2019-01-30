#pragma once
#include "OpenGL.h"
#include "model_generated.h"
#include "Shader.h"
#include <vector>
#include <memory>
#include <string>

class Mesh
{
public:
    Mesh(const ModelData::MeshT & mesh, const std::string & root, Shader & shader, uint32_t flags);
    ~Mesh();

    void BindTextures(const std::vector<GLuint> & textures, const char * name, Shader & shader);
    void Draw(Shader & shader);

private:
    void InitBuffers(const std::vector<ModelData::Vec3> & positions,
        const std::vector<ModelData::Vec3> & normals,
        const std::vector<ModelData::Vec2> & texCoords,
        const std::vector<ModelData::Vec3> & tangents,
        const std::vector<ModelData::Vec3> & bitangents,
        const std::vector<uint16_t> & indices,
        Shader & shader);

    std::vector<GLuint> & GetTextureContainer(ModelData::TextureType type);
    bool InitTextures(const std::vector<std::unique_ptr<ModelData::TextureT>> & textures, const std::string & root);

    const uint32_t m_flags;

    GLuint m_vao;

    struct VBO
    {
        GLuint vbo;
        GLuint index; // or location
    };

    template<class T, uint32_t N>
    friend Mesh::VBO CreateFloatVBO(GLuint index, const std::vector<T> & data, bool isVaoBinded);

    VBO m_positions;
    VBO m_normals;
    VBO m_texCoords;
    VBO m_tangents;
    VBO m_bitangents;

    GLuint m_vboIndices;

    std::vector<GLuint> m_textureDiffuse;
    std::vector<GLuint> m_textureNormal;
    std::vector<GLuint> m_textureSpecular;
    std::vector<GLuint> m_textureAmbient;
    std::vector<GLuint> m_textureHeight;

    GLuint m_verticesCount;
};

class Model
{
public:
    static const uint32_t FlagTextureDiffuse = 0x0001;
    static const uint32_t FlagTextureNormal = 0x0002;
    static const uint32_t FlagTextureSpecular = 0x0004;
    static const uint32_t FlagNormal = 0x0008;
    static const uint32_t FlagVAO = 0x0010;

    Model(const char * path, uint32_t flags, ShaderPtr & shader);
    void Draw();

private:
    std::vector<std::unique_ptr<Mesh>> m_meshes;

    ShaderPtr m_shader;
};