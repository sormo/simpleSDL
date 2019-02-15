#pragma once
#include "OpenGL.h"
#include "model_generated.h"
#include "ModelShader.h"
#include <vector>
#include <memory>
#include <string>

struct ModelMaterial
{
    struct Textures
    {
        std::vector<GLuint> ambient;
        std::vector<GLuint> diffuse;
        std::vector<GLuint> specular;
        std::vector<GLuint> normal;
    };
    Textures textures;
    std::unique_ptr<ModelShader> shader;
};

class Mesh
{
public:

    Mesh(const ModelData::MeshT & mesh, ModelMaterial & material);
    ~Mesh();

    void Draw();

private:
    void InitBuffers(const std::vector<ModelData::Vec3> & positions,
                        const std::vector<ModelData::Vec3> & normals,
                        const std::vector<ModelData::Vec2> & texCoords,
                        const std::vector<ModelData::Vec3> & tangents,
                        const std::vector<ModelData::Vec3> & bitangents,
                        const std::vector<uint16_t> & indices);

    bool InitTextures(const std::vector<std::unique_ptr<ModelData::TextureT>> & textures, const std::string & root);

    GLuint m_vao;

    GLuint m_positions;
    GLuint m_normals;
    std::vector<GLuint> m_texCoords;
    GLuint m_tangents;
    GLuint m_bitangents;

    GLuint m_vboIndices;

    GLuint m_verticesCount;

    ModelMaterial & m_material;
};

class Model
{
public:
    Model(const char * path, ModelShader::ConfigLight light);
    ~Model();

    // bind must be called before drawing
    // camera and light positions should be in world space
    // TODO all meshes are drawn with same shader ?
    void Bind(const ModelShader::Data & data);
    
    void Draw(const glm::mat4 & model, const glm::mat4 & view, const glm::mat4 & projection);

private:
    const ModelShader::ConfigLight m_configLight;

    std::unique_ptr<ModelMaterial> CreateMaterial(const std::string & root, ModelData::MaterialT & material);

    std::vector<std::unique_ptr<Mesh>> m_meshes;
    std::vector<std::unique_ptr<ModelMaterial>> m_materials;
};
