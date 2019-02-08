#pragma once
#include "OpenGL.h"
#include "model_generated.h"
#include "ModelShader.h"
#include <vector>
#include <memory>
#include <string>

class Mesh
{
public:
    Mesh(const ModelData::MeshT & mesh, const std::string & root, ModelShader & shader);
    ~Mesh();

    void Draw();

private:
    void InitBuffers(const std::vector<ModelData::Vec3> & positions,
                        const std::vector<ModelData::Vec3> & normals,
                        const std::vector<ModelData::Vec2> & texCoords,
                        const std::vector<ModelData::Vec3> & tangents,
                        const std::vector<ModelData::Vec3> & bitangents,
                        const std::vector<uint16_t> & indices);

    std::vector<GLuint> & GetTextureContainer(ModelData::TextureType type);
    bool InitTextures(const std::vector<std::unique_ptr<ModelData::TextureT>> & textures, const std::string & root);

    GLuint m_vao;

    GLuint m_positions;
    GLuint m_normals;
    GLuint m_texCoords;
    GLuint m_tangents;
    GLuint m_bitangents;

    GLuint m_vboIndices;

    std::vector<GLuint> m_textureDiffuse;
    std::vector<GLuint> m_textureNormal;
    std::vector<GLuint> m_textureSpecular;
    std::vector<GLuint> m_textureAmbient;
    std::vector<GLuint> m_textureHeight;

    GLuint m_verticesCount;

    ModelShader * m_shader = nullptr;
};

class Model
{
public:
    Model(const char * path, ModelShaderPtr & shader);

    // bind must be called before drawing
    // camera and light positions should be in world space
    // TODO all meshes are drawn with same shader ?
    void Bind(const ModelShader::Data & data);
    
    void Draw(const glm::mat4 & model, const glm::mat4 & view, const glm::mat4 & projection);

private:
    std::vector<std::unique_ptr<Mesh>> m_meshes;

    ModelShaderPtr m_shader;
};
