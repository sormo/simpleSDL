#pragma once
#include "Shader.h"
#include "Shadow.h"

namespace Light
{
    struct Config
    {
        bool directional = false;
        uint32_t pointCount = 0;
        uint32_t spotCount = 0;
    };

    struct Data
    {
        struct Light
        {
            glm::vec3 ambient;
            glm::vec3 diffuse;
            glm::vec3 specular;
        };

        struct LightDirectional : Light
        {
            glm::vec3 direction;
        };

        struct LightPoint : Light
        {
            glm::vec3 position;

            float constant;
            float linear;
            float quadratic;
        };

        struct LightSpot : LightPoint
        {
            glm::vec3 direction;

            float cutOff;
            float outerCutOff;
        };

        LightDirectional lightDirectional;
        std::vector<LightPoint> lightPoint;
        std::vector<LightSpot> lightSpot;
    };

    struct Locations
    {
        // lights
        struct Light
        {
            GLuint ambient;
            GLuint diffuse;
            GLuint specular;
            // shadow specific:
            GLuint depthMap;
            GLuint depthMapSize;
            GLuint lightSpaceMatrix;
            GLuint farPlane;
        };
        struct LightDirectional : Light
        {
            GLuint direction;
        };
        struct LightPoint : Light
        {
            GLuint position;
            GLuint constant;
            GLuint linear;
            GLuint quadratic;
        };
        struct LightSpot : LightPoint
        {
            GLuint direction;
            GLuint cutOff;
            GLuint outerCutOff;
        };
        LightDirectional lightDirectional;
        std::vector<LightPoint> lightPoint;
        std::vector<LightSpot> lightSpot;
    };
}

namespace Material
{
    struct Data
    {
        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;
        float shininess;
        // factor of specular component
        float shininessStrength = 1.0f;
    };

    struct Locations
    {
        GLuint ambient;
        GLuint diffuse;
        GLuint specular;
        GLuint shininess;
        GLuint shininessStrength;
    };
}

namespace Textures
{
    struct Data
    {
        std::vector<GLuint> ambient;
        std::vector<GLuint> diffuse;
        std::vector<GLuint> specular;
        std::vector<GLuint> normal;
        std::vector<GLuint> lightmap;
    };

    struct Locations
    {
        std::vector<GLuint> textureAmbient;
        std::vector<GLuint> textureDiffuse;
        std::vector<GLuint> textureSpecular;
        std::vector<GLuint> textureNormal;
        // TODO lightmaps are not yet used in shader
        std::vector<GLuint> textureLightmaps;
    };
}

namespace Buffers
{
    struct Locations
    {
        GLuint positions;
        GLuint normals;
        std::vector<GLuint> texCoords;
        GLuint tangents;
        GLuint bitangents;
    };
}

class ModelShader
{
public:
    struct TextureStackEntry
    {
        enum class Operation
        {
            // T = T1 * T2
            Multiply = 0,
            // T = T1 + T2
            Add = 0x1,
            // T = T1 - T2
            Substract = 0x2,
            // T = T1 / T2
            Divide = 0x3,
            // T = (T1 + T2) - (T1 * T2)
            SmoothAdd = 0x4,
            // T = T1 + (T2-0.5)
            SignedAdd = 0x5,
        };

        float factor;
        Operation operation;
        uint32_t uvIndex;
    };

    struct TextureMaps
    {
        std::vector<TextureStackEntry> ambient;
        std::vector<TextureStackEntry> diffuse;
        std::vector<TextureStackEntry> specular;
        std::vector<TextureStackEntry> normal;
        std::vector<TextureStackEntry> lightmap;
    };

    enum class ShadingModel
    {
        Phong = 0,
        BlinnPhong
    };

    struct Config
    {
        enum class Flags : uint32_t
        {
            // material will be binded at runtime from data
            UseRuntimeMaterial = 0x0001
        };

        Material::Data material;

        TextureMaps textures;

        Light::Config light;

        uint32_t flags = 0;

        ShadingModel shading = ShadingModel::Phong;

        // number of color attributes per vertex
        // TODO not used don't know what to do with it in shader
        uint32_t colorsCount = 0;

        // determines number of UV attributes per vertex
        // TODO simplify computation
        uint32_t GetUVChannelsCount() const;
    };

    struct Locations
    {
        Buffers::Locations buffers;

        GLuint cameraWorldSpace;

        Material::Locations material;
        Light::Locations light;
        Textures::Locations textures;
    };

    ModelShader(Config config);

    const Config & GetConfig();
    const Locations & GetLocations();
    Shader & GetShader();

    void BeginRender();
    void EndRender();

    // Render passes for each light.
    // While BeginRenderShadow returns true, scene must be rendered like this:
    // while(BeginRenderShadow(lightData))
    // {
    //      ... draw scene
    //      ... for each model call BindShadowTransform
    //      EndRenderShadow();
    // }
    // After that bind data and draw scene.
    // Begin render shadow must be called after BeginRender()
    bool BeginRenderShadow(const Light::Data & data);
    void EndRenderShadow();

    // Also buffers must be bound like this (vao or vbo):
    //      shader.GetShader().BindVAO(vao);
    //      shader.GetShader().BindBuffer<glm::vec3>(vbo, shader.GetLocations().buffers.positions);
    // See Buffers::Locations structure.
    void BindTransform(const glm::mat4 & model, const glm::mat4 & view, const glm::mat4 & projection);
    void BindTransformShadow(const glm::mat4 & model);
    void BindMaterial(const Material::Data & data);
    void BindLight(const Light::Data & data);
    void BindTextures(const Textures::Data & data);
    void BindCamera(const glm::vec3 & cameraWorldSpace);

private:
    template<class T, class L>
    void Bind(const T & data, const L & locations);

    template<class T>
    void InitLocation(const std::string & path, T & location);

    void InitTextureLocations(const std::string & path, uint32_t count, std::vector<GLuint> & locations);

    void InitModelLocations();
    void InitMeshLocations();

    Config m_config;
    Locations m_locations;
    std::unique_ptr<Shader> m_shader;

    struct ShadowState
    {
        bool directionalState;
        uint32_t pointCounter;
        uint32_t spotCounter;

        std::unique_ptr<ShadowDirectionalLight> directional;
        std::vector<std::unique_ptr<ShadowPointLight>> point;
        std::vector<std::unique_ptr<ShadowSpotLight>> spot;

    } m_shadows;
    void ResetShadowState();
    void InitShadows();
    void BindShadows();
};

using ModelShaderPtr = std::shared_ptr<ModelShader>;
