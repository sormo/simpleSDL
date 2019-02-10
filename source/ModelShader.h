#pragma once
#include "Shader.h"

class ModelShader
{
public:
    struct Config
    {
        uint32_t textureAmbientCount = 0;
        uint32_t textureDiffuseCount = 0;
        uint32_t textureSpecularCount = 0;
        uint32_t textureNormalCount = 0;

        bool lightDirection = false;
        uint32_t lightPointCount = 0;
        uint32_t lightSpotCount = 0;

        // determines whether this config needs UV coordinates
        bool NeedsUVs() const;
    };

    struct Locations
    {
        // VBOs
        GLuint positions;
        GLuint normals;
        GLuint texCoords;
        GLuint tangents;
        GLuint bitangents;

        struct Material
        {
            GLuint ambient;
            GLuint diffuse;
            GLuint specular;
        };

        // material
        Material material;

        GLuint shininess;

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
        struct Material
        {
            glm::vec3 ambient;
            glm::vec3 diffuse;
            glm::vec3 specular;
        };

        Material material;
        float shininess;
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
