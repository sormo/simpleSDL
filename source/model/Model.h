#pragma once
#include "OpenGL.h"
#include "model_generated.h"
#include "ModelShader.h"
#include <vector>
#include <memory>
#include <string>

struct ModelMaterial
{
    Textures::Data textures;
    std::unique_ptr<ModelShader> shader;
};

class Mesh
{
public:

    Mesh(const ModelData::MeshT & mesh, std::vector<std::unique_ptr<ModelMaterial>> & materials);
    ~Mesh();

    void Draw(const glm::mat4 & model, const glm::mat4 & view, const glm::mat4 & projection);

private:
    void InitBuffers(const std::vector<ModelData::Vec3> & positions,
                        const std::vector<ModelData::Vec3> & normals,
                        const std::vector<ModelData::Vec2> & texCoords,
                        const std::vector<ModelData::Vec3> & tangents,
                        const std::vector<ModelData::Vec3> & bitangents,
                        const std::vector<uint16_t> & indices);

    // bind mesh specific uniforms
    void BindUniforms(const glm::mat4 & model, const glm::mat4 & view, const glm::mat4 & projection);

    void BindBuffers();

    GLuint m_vao;

    GLuint m_positions;
    GLuint m_normals;
    std::vector<GLuint> m_texCoords;
    GLuint m_tangents;
    GLuint m_bitangents;

    GLuint m_vboIndices;

    GLuint m_verticesCount;

    ModelMaterial * m_material;
};

class Model
{
public:
    Model(const char * path, Light::Config light);
    ~Model();

    struct Data
    {
        Material::Data material;
        Light::Data light;
        glm::vec3 cameraWorldSpace;
    };

    // bind must be called before drawing
    // camera and light positions should be in world space
    void Bind(const Data & data);
    
    void Draw(const glm::mat4 & model, const glm::mat4 & view, const glm::mat4 & projection);

private:
    struct Tree
    {
        glm::mat4 transform;
        std::vector<uint32_t> meshes;
        std::vector<Tree> childs;
    };

    void ProcessMaterials(ModelData::ModelT * model, const std::string & root);
    void ProcessMeshes(ModelData::ModelT * model);
    Tree ProcessTree(ModelData::TreeT & node);

    void DrawInternal(Tree & tree, const glm::mat4 & model, const glm::mat4 & view, const glm::mat4 & projection);

    const Light::Config m_configLight;

    std::unique_ptr<ModelMaterial> CreateMaterial(const std::string & root, ModelData::MaterialT & material);
    std::vector<std::unique_ptr<ModelMaterial>> m_materials;

    std::vector<std::unique_ptr<Mesh>> m_meshes;


    Tree m_tree;
};
