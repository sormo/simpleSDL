#include "ModelShader.h"

template<class T>
void ModelShader::InitLocation(const std::string & path, T & location)
{
    printf("InitLocation call to invalid type in ModelShader.");
    throw std::runtime_error("InitLocation call to invalid type in ModelShader.");
}

template<>
void ModelShader::InitLocation<ModelShader::Locations::Light>(const std::string & path, ModelShader::Locations::Light & location)
{
    location.ambient = m_shader.GetLocation(path + "ambient", Shader::LocationType::Uniform);
    location.diffuse = m_shader.GetLocation(path + "diffuse", Shader::LocationType::Uniform);
    location.specular = m_shader.GetLocation(path + "specular", Shader::LocationType::Uniform);
}

template<>
void ModelShader::InitLocation<ModelShader::Locations::LightDirectional>(const std::string & path, ModelShader::Locations::LightDirectional & location)
{
    InitLocation<ModelShader::Locations::Light>(path, location);
    location.direction = m_shader.GetLocation(path + "direction", Shader::LocationType::Uniform);
}

template<>
void ModelShader::InitLocation<ModelShader::Locations::LightPoint>(const std::string & path, ModelShader::Locations::LightPoint & location)
{
    InitLocation<ModelShader::Locations::Light>(path, location);
    location.position = m_shader.GetLocation(path + "position", Shader::LocationType::Uniform);
    location.constant = m_shader.GetLocation(path + "constant", Shader::LocationType::Uniform);
    location.linear = m_shader.GetLocation(path + "linear", Shader::LocationType::Uniform);
    location.quadratic = m_shader.GetLocation(path + "quadratic", Shader::LocationType::Uniform);
}

template<>
void ModelShader::InitLocation<ModelShader::Locations::LightSpot>(const std::string & path, ModelShader::Locations::LightSpot & location)
{
    InitLocation<ModelShader::Locations::LightPoint>(path, location);
    location.direction = m_shader.GetLocation(path + "direction", Shader::LocationType::Uniform);
    location.cutOff = m_shader.GetLocation(path + "cutOff", Shader::LocationType::Uniform);
    location.outerCutOff = m_shader.GetLocation(path + "outerCutOff", Shader::LocationType::Uniform);
}

template<>
void ModelShader::InitLocation<ModelShader::Locations::Material>(const std::string & path, ModelShader::Locations::Material & location)
{
    location.ambient = m_shader.GetLocation(path + "ambient", Shader::LocationType::Uniform);
    location.diffuse = m_shader.GetLocation(path + "diffuse", Shader::LocationType::Uniform);
    location.specular = m_shader.GetLocation(path + "specular", Shader::LocationType::Uniform);
}

ModelShader::ModelShader(const char * vertexFile, const char * fragmentFile, Config config)
    : m_shader(vertexFile, fragmentFile), m_config(config)
{
    if (m_shader)
    {
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
    return m_shader;
}

bool ModelShader::Config::NeedsUVs() const
{
    return textureAmbientCount || textureDiffuseCount || textureNormalCount || textureSpecularCount;
}

void ModelShader::InitTextureLocations(const std::string & path, uint32_t count, std::vector<GLuint> & locations)
{
    for (uint32_t i = 0; i < count; ++i)
        locations.push_back(m_shader.GetLocation(path + "[" + std::to_string(i) + "]", Shader::LocationType::Uniform));
}

void ModelShader::InitMeshLocations()
{
    // VBOs
    m_locations.positions = m_shader.GetLocation("positionModelSpace", Shader::LocationType::Attrib);
    m_locations.normals = m_shader.GetLocation("normalModelSpace", Shader::LocationType::Attrib);
    if (m_config.NeedsUVs())
        m_locations.texCoords = m_shader.GetLocation("vertexUV", Shader::LocationType::Attrib);
    if (m_config.textureNormalCount)
    {
        m_locations.tangents = m_shader.GetLocation("tangentModelSpace", Shader::LocationType::Attrib);
        //m_locations.bitangents = m_shader.GetLocation("bitangentModelSpace", Shader::LocationType::Attrib);
    }

    // textures
    InitTextureLocations("textureAmbient", m_config.textureAmbientCount, m_locations.textureAmbient);
    InitTextureLocations("textureDiffuse", m_config.textureDiffuseCount, m_locations.textureDiffuse);
    InitTextureLocations("textureSpecular", m_config.textureSpecularCount, m_locations.textureSpecular);
    InitTextureLocations("textureNormal", m_config.textureNormalCount, m_locations.textureNormal);
}

void ModelShader::InitModelLocations()
{
    if (m_config.lightDirection)
        InitLocation("lightDirectional.", m_locations.lightDirectional);

    for (size_t i = 0; i < m_config.lightPointCount; ++i)
    {
        m_locations.lightPoint.push_back({});
        InitLocation("lightPoint[" + std::to_string(i) + "].", m_locations.lightPoint.back());
    }

    for (size_t i = 0; i < m_config.lightSpotCount; ++i)
    {
        m_locations.lightSpot.push_back({});
        InitLocation("lightSpot[" + std::to_string(i) + "].", m_locations.lightSpot.back());
    }

    if (!m_config.textureAmbientCount)
        m_locations.material.ambient = m_shader.GetLocation("material.ambient", Shader::LocationType::Uniform);
    if (!m_config.textureDiffuseCount)
        m_locations.material.diffuse = m_shader.GetLocation("material.diffuse", Shader::LocationType::Uniform);
    if (!m_config.textureSpecularCount)
        m_locations.material.specular = m_shader.GetLocation("material.specular", Shader::LocationType::Uniform);

    m_locations.shininess = m_shader.GetLocation("shininess", Shader::LocationType::Uniform);
    m_locations.cameraWorldSpace = m_shader.GetLocation("cameraWorldSpace", Shader::LocationType::Uniform);
}

template<class T, class L>
void ModelShader::Bind(const T & data, const L & locations)
{
    printf("Bind call to invalid type in ModelShader.");
    throw std::runtime_error("Bind call to invalid type in ModelShader.");
}

template<>
void ModelShader::Bind<ModelShader::Data::Light, ModelShader::Locations::Light>(const ModelShader::Data::Light & data, const ModelShader::Locations::Light & locations)
{
    m_shader.SetUniform(data.ambient, locations.ambient);
    m_shader.SetUniform(data.diffuse, locations.diffuse);
    m_shader.SetUniform(data.specular, locations.specular);
}

template<>
void ModelShader::Bind<ModelShader::Data::LightDirectional, ModelShader::Locations::LightDirectional>(const ModelShader::Data::LightDirectional & data, const ModelShader::Locations::LightDirectional & locations)
{
    Bind<ModelShader::Data::Light, ModelShader::Locations::Light>(data, locations);

    m_shader.SetUniform(data.direction, locations.direction);
}

template<>
void ModelShader::Bind<ModelShader::Data::LightPoint, ModelShader::Locations::LightPoint>(const ModelShader::Data::LightPoint & data, const ModelShader::Locations::LightPoint & locations)
{
    Bind<ModelShader::Data::Light, ModelShader::Locations::Light>(data, locations);

    m_shader.SetUniform(data.position, locations.position);
    m_shader.SetUniform(data.constant, locations.constant);
    m_shader.SetUniform(data.linear, locations.linear);
    m_shader.SetUniform(data.quadratic, locations.quadratic);
}

template<>
void ModelShader::Bind<ModelShader::Data::LightSpot, ModelShader::Locations::LightSpot>(const ModelShader::Data::LightSpot & data, const ModelShader::Locations::LightSpot & locations)
{
    Bind<ModelShader::Data::LightPoint, ModelShader::Locations::LightPoint>(data, locations);

    m_shader.SetUniform(data.cutOff, locations.cutOff);
    m_shader.SetUniform(data.outerCutOff, locations.outerCutOff);
    m_shader.SetUniform(data.direction, locations.direction);
}

template<>
void ModelShader::Bind<ModelShader::Data::Material, ModelShader::Locations::Material>(const ModelShader::Data::Material & data, const ModelShader::Locations::Material & locations)
{
    m_shader.SetUniform(data.ambient, locations.ambient);
    m_shader.SetUniform(data.diffuse, locations.diffuse);
    m_shader.SetUniform(data.specular, locations.specular);
}

void ModelShader::Bind(const Data & data)
{
    m_shader.Begin();

    if (m_config.lightDirection)
        Bind(data.lightDirectional, m_locations.lightDirectional);

    for (size_t i = 0; i < m_config.lightPointCount; ++i)
        Bind(data.lightPoint[i], m_locations.lightPoint[i]);

    for (size_t i = 0; i < m_config.lightSpotCount; ++i)
        Bind(data.lightSpot[i], m_locations.lightSpot[i]);

    if (!m_config.textureAmbientCount)
        m_shader.SetUniform(data.material.ambient, m_locations.material.ambient);
    if (!m_config.textureDiffuseCount)
        m_shader.SetUniform(data.material.diffuse, m_locations.material.diffuse);
    if (!m_config.textureSpecularCount)
        m_shader.SetUniform(data.material.specular, m_locations.material.specular);

    m_shader.SetUniform(data.shininess, m_locations.shininess);
    m_shader.SetUniform(data.cameraWorldSpace, m_locations.cameraWorldSpace);
}
