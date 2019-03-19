#include "ModelShader.h"
#include "ShaderGenerator.h"
#include <algorithm>

template<class T>
void ModelShader::InitLocation(const std::string & path, T & location)
{
    printf("InitLocation call to invalid type in ModelShader.");
    throw std::runtime_error("InitLocation call to invalid type in ModelShader.");
}

template<>
void ModelShader::InitLocation<Light::Locations::Light>(const std::string & path, Light::Locations::Light & location)
{
    location.ambient = m_shader->GetLocation(path + "ambient", Shader::LocationType::Uniform);
    location.diffuse = m_shader->GetLocation(path + "diffuse", Shader::LocationType::Uniform);
    location.specular = m_shader->GetLocation(path + "specular", Shader::LocationType::Uniform);
}

template<>
void ModelShader::InitLocation<Light::Locations::LightDirectional>(const std::string & path, Light::Locations::LightDirectional & location)
{
    InitLocation<Light::Locations::Light>(path, location);
    location.direction = m_shader->GetLocation(path + "direction", Shader::LocationType::Uniform);
}

template<>
void ModelShader::InitLocation<Light::Locations::LightPoint>(const std::string & path, Light::Locations::LightPoint & location)
{
    InitLocation<Light::Locations::Light>(path, location);
    location.position = m_shader->GetLocation(path + "position", Shader::LocationType::Uniform);
    location.constant = m_shader->GetLocation(path + "constant", Shader::LocationType::Uniform);
    location.linear = m_shader->GetLocation(path + "linear", Shader::LocationType::Uniform);
    location.quadratic = m_shader->GetLocation(path + "quadratic", Shader::LocationType::Uniform);
}

template<>
void ModelShader::InitLocation<Light::Locations::LightSpot>(const std::string & path, Light::Locations::LightSpot & location)
{
    InitLocation<Light::Locations::LightPoint>(path, location);
    location.direction = m_shader->GetLocation(path + "direction", Shader::LocationType::Uniform);
    location.cutOff = m_shader->GetLocation(path + "cutOff", Shader::LocationType::Uniform);
    location.outerCutOff = m_shader->GetLocation(path + "outerCutOff", Shader::LocationType::Uniform);
}

template<>
void ModelShader::InitLocation<Material::Locations>(const std::string & path, Material::Locations & location)
{
    location.ambient = m_shader->GetLocation(path + "ambient", Shader::LocationType::Uniform);
    location.diffuse = m_shader->GetLocation(path + "diffuse", Shader::LocationType::Uniform);
    location.specular = m_shader->GetLocation(path + "specular", Shader::LocationType::Uniform);
    location.shininess = m_shader->GetLocation(path + "shininess", Shader::LocationType::Uniform);
    location.shininessStrength = m_shader->GetLocation(path + "shininessStrength", Shader::LocationType::Uniform);
}

ModelShader::ModelShader(Config config)
    : m_config(config)
{
    auto shaders = ShaderGenerator::Generate(config);

    m_shader = std::make_unique<Shader>(shaders.vertex.c_str(), shaders.fragment.c_str());

    if (m_shader)
    {
        m_shader->PrintUniforms();
        m_shader->PrintAttributes();

        InitModelLocations();
        InitMeshLocations();
    }
}

const ModelShader::Config & ModelShader::GetConfig()
{
    return m_config;
}

const ModelShader::Locations & ModelShader::GetLocations()
{
    return m_locations;
}

Shader & ModelShader::GetShader()
{
    return *m_shader;
}

uint32_t ModelShader::Config::GetUVChannelsCount() const
{
    uint32_t result = 0;

    auto getMaxChannel = [](const std::vector<TextureStackEntry> & textureStack) -> uint32_t
    {
        uint32_t result = 0;
        for (const auto & entry : textureStack)
            result = std::max(result, entry.uvIndex + 1);
        return result;
    };

    result = std::max(result, getMaxChannel(textures.ambient));
    result = std::max(result, getMaxChannel(textures.diffuse));
    result = std::max(result, getMaxChannel(textures.specular));
    result = std::max(result, getMaxChannel(textures.normal));

    return result;
}

void ModelShader::InitTextureLocations(const std::string & path, uint32_t count, std::vector<GLuint> & locations)
{
    for (uint32_t i = 0; i < count; ++i)
        locations.push_back(m_shader->GetLocation(path + "[" + std::to_string(i) + "]", Shader::LocationType::Uniform));
}

void ModelShader::InitMeshLocations()
{
    // VBOs
    m_locations.buffers.positions = m_shader->GetLocation("positionModelSpace", Shader::LocationType::Attrib);
    m_locations.buffers.normals = m_shader->GetLocation("normalModelSpace", Shader::LocationType::Attrib);
    for (uint32_t i = 0; i < m_config.GetUVChannelsCount(); ++i)
        m_locations.buffers.texCoords.push_back(m_shader->GetLocation("vertexUV" + std::to_string(i), Shader::LocationType::Attrib));
    if (m_config.textures.normal.size())
    {
        m_locations.buffers.tangents = m_shader->GetLocation("tangentModelSpace", Shader::LocationType::Attrib);
        //m_locations.buffers.bitangents = m_shader->GetLocation("bitangentModelSpace", Shader::LocationType::Attrib);
    }

    // textures
    InitTextureLocations("textureAmbient", m_config.textures.ambient.size(), m_locations.textures.textureAmbient);
    InitTextureLocations("textureDiffuse", m_config.textures.diffuse.size(), m_locations.textures.textureDiffuse);
    InitTextureLocations("textureSpecular", m_config.textures.specular.size(), m_locations.textures.textureSpecular);
    InitTextureLocations("textureNormal", m_config.textures.normal.size(), m_locations.textures.textureNormal);
    InitTextureLocations("textureLightmap", m_config.textures.lightmap.size(), m_locations.textures.textureLightmaps);
}

void ModelShader::InitModelLocations()
{
    if (m_config.light.directional)
        InitLocation("lightDirectional.", m_locations.light.lightDirectional);

    for (size_t i = 0; i < m_config.light.pointCount; ++i)
    {
        m_locations.light.lightPoint.push_back({});
        InitLocation("lightPoint[" + std::to_string(i) + "].", m_locations.light.lightPoint.back());
    }

    for (size_t i = 0; i < m_config.light.spotCount; ++i)
    {
        m_locations.light.lightSpot.push_back({});
        InitLocation("lightSpot[" + std::to_string(i) + "].", m_locations.light.lightSpot.back());
    }

    if (m_config.flags & (uint32_t)Config::Flags::UseRuntimeMaterial)
        InitLocation("material.", m_locations.material);

    m_locations.cameraWorldSpace = m_shader->GetLocation("cameraWorldSpace", Shader::LocationType::Uniform);
}

template<class T, class L>
void ModelShader::Bind(const T & data, const L & locations)
{
    printf("Bind call to invalid type in ModelShader.");
    throw std::runtime_error("Bind call to invalid type in ModelShader.");
}

template<>
void ModelShader::Bind<Light::Data::Light, Light::Locations::Light>(const Light::Data::Light & data, const Light::Locations::Light & locations)
{
    m_shader->SetUniform(data.ambient, locations.ambient);
    m_shader->SetUniform(data.diffuse, locations.diffuse);
    m_shader->SetUniform(data.specular, locations.specular);
}

template<>
void ModelShader::Bind<Light::Data::LightDirectional, Light::Locations::LightDirectional>(const Light::Data::LightDirectional & data, const Light::Locations::LightDirectional & locations)
{
    Bind<Light::Data::Light, Light::Locations::Light>(data, locations);

    m_shader->SetUniform(data.direction, locations.direction);
}

template<>
void ModelShader::Bind<Light::Data::LightPoint, Light::Locations::LightPoint>(const Light::Data::LightPoint & data, const Light::Locations::LightPoint & locations)
{
    Bind<Light::Data::Light, Light::Locations::Light>(data, locations);

    m_shader->SetUniform(data.position, locations.position);
    m_shader->SetUniform(data.constant, locations.constant);
    m_shader->SetUniform(data.linear, locations.linear);
    m_shader->SetUniform(data.quadratic, locations.quadratic);
}

template<>
void ModelShader::Bind<Light::Data::LightSpot, Light::Locations::LightSpot>(const Light::Data::LightSpot & data, const Light::Locations::LightSpot & locations)
{
    Bind<Light::Data::LightPoint, Light::Locations::LightPoint>(data, locations);

    m_shader->SetUniform(data.cutOff, locations.cutOff);
    m_shader->SetUniform(data.outerCutOff, locations.outerCutOff);
    m_shader->SetUniform(data.direction, locations.direction);
}

template<>
void ModelShader::Bind<Material::Data, Material::Locations>(const Material::Data & data, const Material::Locations & locations)
{
    m_shader->SetUniform(data.ambient, locations.ambient);
    m_shader->SetUniform(data.diffuse, locations.diffuse);
    m_shader->SetUniform(data.specular, locations.specular);
    m_shader->SetUniform(data.shininess, locations.shininess);
    m_shader->SetUniform(data.shininessStrength, locations.shininessStrength);
}

void ModelShader::BeginRender()
{
    m_shader->BeginRender();
}

void ModelShader::EndRender()
{
    m_shader->EndRender();
}

void ModelShader::BindMaterial(const Material::Data & data)
{
    Bind(data, m_locations.material);
}

void ModelShader::BindLight(const Light::Data & data)
{
    if (m_config.light.directional)
        Bind(data.lightDirectional, m_locations.light.lightDirectional);

    for (size_t i = 0; i < m_config.light.pointCount; ++i)
        Bind(data.lightPoint[i], m_locations.light.lightPoint[i]);

    for (size_t i = 0; i < m_config.light.spotCount; ++i)
        Bind(data.lightSpot[i], m_locations.light.lightSpot[i]);
}

void ModelShader::BindCamera(const glm::vec3 & cameraWorldSpace)
{
    m_shader->SetUniform(cameraWorldSpace, m_locations.cameraWorldSpace);
}

void ModelShader::BindTransform(const glm::mat4 & model, const glm::mat4 & view, const glm::mat4 & projection)
{
    glm::mat4 MVP = projection * view * model;

    m_shader->SetUniform(MVP, "MVP");
    m_shader->SetUniform(model, "M");
}

void ModelShader::BindTextures(const Textures::Data & data)
{
    for (size_t i = 0; i < m_config.textures.ambient.size(); ++i)
        m_shader->BindTexture(data.ambient[i], m_locations.textures.textureAmbient[i]);

    for (size_t i = 0; i < m_config.textures.diffuse.size(); ++i)
        m_shader->BindTexture(data.diffuse[i], m_locations.textures.textureDiffuse[i]);

    for (size_t i = 0; i < m_config.textures.specular.size(); ++i)
        m_shader->BindTexture(data.specular[i], m_locations.textures.textureSpecular[i]);

    for (size_t i = 0; i < m_config.textures.normal.size(); ++i)
        m_shader->BindTexture(data.normal[i], m_locations.textures.textureNormal[i]);

    for (size_t i = 0; i < m_config.textures.lightmap.size(); ++i)
        m_shader->BindTexture(data.lightmap[i], m_locations.textures.textureLightmaps[i]);
}
