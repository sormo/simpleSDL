#pragma once
#include "Shader.h"

class ModelShader
{
public:
    // right now lights are part of shader config which is possibly not necessary
    struct ConfigLight
    {
        bool directional = false;
        uint32_t pointCount = 0;
        uint32_t spotCount = 0;
    };

    struct Material
    {
        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;
        float shininess;
        // factor of specular component
        float shininessStrength = 1.0f;
    };

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
    };

    struct Config
    {
        enum class Flags : uint32_t
        {
            // material will be binded at runtime from data
            UseRuntimeMaterial = 0x0001
        };

        Material material;

        TextureMaps textures;

        ConfigLight light;

        uint32_t flags = 0;

        // number of color attributes per vertex
        // TODO not used don't know what to do with it in shader
        uint32_t colorsCount = 0;

        // determines number of UV attributes per vertex
        // TODO simplify computation
        uint32_t GetUVChannelsCount() const;
    };

    struct Locations
    {
        // VBOs
        GLuint positions;
        GLuint normals;
        std::vector<GLuint> texCoords;
        GLuint tangents;
        GLuint bitangents;
        // std::vector<GLuint> colors;

        struct Material
        {
            GLuint ambient;
            GLuint diffuse;
            GLuint specular;
            GLuint shininess;
        };

        // material
        Material material;

        GLuint cameraWorldSpace;

        // textures
        std::vector<GLuint> textureAmbient;
        std::vector<GLuint> textureDiffuse;
        std::vector<GLuint> textureSpecular;
        std::vector<GLuint> textureNormal;

        // lights
        struct Light
        {
            GLuint ambient;
            GLuint diffuse;
            GLuint specular;
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

    struct Data
    {
        Material material;

        glm::vec3 cameraWorldSpace;

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

        LightDirectional lightDirectional;

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

        std::vector<LightPoint> lightPoint;
        std::vector<LightSpot> lightSpot;
    };

    ModelShader(Config config);

    const Config & GetConfig();
    const Locations & GetLocations();
    Shader & GetShader();

    void Bind(const Data & data);

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
};

using ModelShaderPtr = std::shared_ptr<ModelShader>;
