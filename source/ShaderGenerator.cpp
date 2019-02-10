#include "ShaderGenerator.h"

void AppendHeader(std::string & result)
{
    result += "#version 100\n\n";
    result += "#ifdef GL_ES\n";
    result += "precision mediump float;\n";
    result += "#endif\n\n";
}

void AppendAttributes(const ModelShader::Config & config, std::string & result)
{
    result += "// *** attributes ***\n";
    result += "attribute vec3 positionModelSpace;\n";
    result += "attribute vec3 normalModelSpace;\n";
    if (config.NeedsUVs())
        result += "attribute vec2 vertexUV;\n";
    if (config.textureNormalCount)
    {
        result += "attribute vec3 tangentModelSpace;\n";
        //result += "attribute vec3 bitangentModelSpace;\n";
    }
    result += "// ******************\n";
}

void AppendUniforms(const ModelShader::Config & config, std::string & result)
{
    result += "// *** general uniforms ***\n";
    result += "uniform mat4 MVP;\n";
    result += "uniform mat4 M;\n";
    result += "uniform vec3 cameraWorldSpace;\n";
    result += "// ************************\n";
}

void AppendUniformsMaterial(const ModelShader::Config & config, std::string & result)
{
    result += "// *** material uniforms ***\n";
    result += "struct Material\n";
    result += "{\n";
    result += "    vec3 ambient;\n";
    result += "    vec3 diffuse;\n";
    result += "    vec3 specular;\n";
    result += "};\n";
    result += "uniform Material material;\n";
    result += "uniform float shininess;\n";
    if (config.textureAmbientCount)
        result += "uniform sampler2D textureAmbient[" + std::to_string(config.textureAmbientCount) + "];\n";
    if (config.textureDiffuseCount)
        result += "uniform sampler2D textureDiffuse[" + std::to_string(config.textureDiffuseCount) + "];\n";
    if (config.textureSpecularCount)
        result += "uniform sampler2D textureSpecular[" + std::to_string(config.textureSpecularCount) + "];\n";
    if (config.textureNormalCount)
        result += "uniform sampler2D textureNormal[" + std::to_string(config.textureNormalCount) + "];\n";
    result += "// *************************\n";
}

void AppendUniformsLight(const ModelShader::Config & config, std::string & result)
{
    result += "// *** light uniforms ***\n";
    if (config.lightDirection)
    {
        result += "struct LightDirectional\n";
        result += "{\n";
        result += "    vec3 direction;\n";
        result += "    vec3 ambient;\n";
        result += "    vec3 diffuse;\n";
        result += "    vec3 specular;\n";
        result += "};\n";
        result += "uniform LightDirectional lightDirectional;\n";
    }
    if (config.lightPointCount)
    {
        result += "struct LightPoint\n";
        result += "{\n";
        result += "    vec3 position;\n";
        result += "    vec3 ambient;\n";
        result += "    vec3 diffuse;\n";
        result += "    vec3 specular;\n";
        result += "    float constant;\n";
        result += "    float linear;\n";
        result += "    float quadratic;\n";
        result += "};\n";
        result += "uniform LightPoint lightPoint[" + std::to_string(config.lightPointCount) + "];\n";
    }
    if (config.lightSpotCount)
    {
        result += "struct LightSpot\n";
        result += "{\n";
        result += "    vec3 position;\n";
        result += "    vec3 direction;\n";
        result += "    float cutOff;\n";
        result += "    float outerCutOff;\n";
        result += "    vec3 ambient;\n";
        result += "    vec3 diffuse;\n";
        result += "    vec3 specular;\n";
        result += "    float constant;\n";
        result += "    float linear;\n";
        result += "    float quadratic;\n";
        result += "};\n";
        result += "uniform LightSpot lightSpot[" + std::to_string(config.lightSpotCount) + "];\n";
    }
    result += "// **********************\n";
}

void AppendVaryings(const ModelShader::Config & config, std::string & result)
{
    result += "// *** varyings ***\n";
    result += "varying vec3 positionWorldSpace;\n";
    if (config.textureNormalCount)
        result += "varying mat3 TBN;\n";
    else
        result += "varying vec3 normalWorldSpace;\n";
    if (config.NeedsUVs())
        result += "varying vec2 vertexUVA;\n";
    result += "// ****************\n";
}

void AppendFunctionSupport(std::string & result)
{
    result += "// *** support functions ***\n";
    result += "mat3 transposeCustom(mat3 m)\n";
    result += "{\n";
    result += "    vec3 i0 = m[0];\n";
    result += "    vec3 i1 = m[1];\n";
    result += "    vec3 i2 = m[2];\n\n";
    result += "    return mat3(vec3(i0.x, i1.x, i2.x),\n";
    result += "                vec3(i0.y, i1.y, i2.y),\n";
    result += "                vec3(i0.z, i1.z, i2.z));\n";
    result += "}\n\n";
    result += "mat3 inverseCustom(mat3 m)\n";
    result += "{\n";
    result += "  float a00 = m[0][0], a01 = m[0][1], a02 = m[0][2];\n";
    result += "  float a10 = m[1][0], a11 = m[1][1], a12 = m[1][2];\n";
    result += "  float a20 = m[2][0], a21 = m[2][1], a22 = m[2][2];\n\n";
    result += "  float b01 =  a22 * a11 - a12 * a21;\n";
    result += "  float b11 = -a22 * a10 + a12 * a20;\n";
    result += "  float b21 =  a21 * a10 - a11 * a20;\n\n";
    result += "  float det = a00 * b01 + a01 * b11 + a02 * b21;\n\n";
    result += "  return mat3(b01, (-a22 * a01 + a02 * a21), ( a12 * a01 - a02 * a11),\n";
    result += "              b11, ( a22 * a00 - a02 * a20), (-a12 * a00 + a02 * a10),\n";
    result += "              b21, (-a21 * a00 + a01 * a20), ( a11 * a00 - a01 * a10)) / det;\n";
    result += "}\n";
    result += "// *************************\n";
}

void AppendBodyVertex(const ModelShader::Config & config, std::string & result)
{
    result += "// *** vertex body ***\n";
    result += "void main(void)\n";
    result += "{\n";
    result += "  gl_Position =  MVP * vec4(positionModelSpace, 1.0);\n";
    result += "  positionWorldSpace = vec3(M * vec4(positionModelSpace, 1.0));\n\n";
    result += "  mat3 normalMatrix = transposeCustom(inverseCustom(mat3(M)));\n";
    if (config.textureNormalCount)
    {
        result += "  vec3 T = normalize(normalMatrix * tangentModelSpace);\n";
        result += "  vec3 N = normalize(normalMatrix * normalModelSpace);\n";
        result += "  T = normalize(T - dot(T, N) * N);\n";
        result += "  vec3 B = normalize(cross(N, T));\n";
        result += "  TBN = mat3(T, B, N);\n\n";
    }
    else
    {
        result += "  normalWorldSpace = normalMatrix * normalModelSpace;\n\n";
    }
    if (config.NeedsUVs())
        result += "  vertexUVA = vertexUV;\n";
    result += "}\n";
    result += "// ******************\n";
}

void AppendFunctionLightDirectional(std::string & result)
{
    result += "vec3 CalcLightDirectional(LightDirectional light, vec3 ambientColor, vec3 diffuseColor, vec3 specularColor, vec3 cameraDirectionWorldSpace, vec3 normal)\n";
    result += "{\n";
    result += "    vec3 ligthDirectionWorldSpace = normalize(-light.direction);\n";
    result += "    // diffuse shading\n";
    result += "    float diffuseFactor = max(dot(normalize(normal), ligthDirectionWorldSpace), 0.0);\n";
    result += "    // specular shading\n";
    result += "    vec3 reflectDirectionWorldSpace = reflect(-ligthDirectionWorldSpace, normalize(normal));\n";
    result += "    float specularFactor = pow(max(dot(cameraDirectionWorldSpace, reflectDirectionWorldSpace), 0.0), shininess);\n";
    result += "    // combine results\n";
    result += "    vec3 ambient = light.ambient * ambientColor;\n";
    result += "    vec3 diffuse = light.diffuse * diffuseFactor * diffuseColor;\n";
    result += "    vec3 specular = light.specular * specularFactor * specularColor;\n\n";
    result += "    return (ambient + diffuse + specular);\n";
    result += "}\n";
}

void AppendFunctionLightPoint(std::string & result)
{
    result += "vec3 CalcLightPoint(LightPoint light, vec3 ambientColor, vec3 diffuseColor, vec3 specularColor, vec3 cameraDirectionWorldSpace, vec3 normal)\n";
    result += "{\n";
    result += "    vec3 ligthDirectionWorldSpace = normalize(light.position - positionWorldSpace);\n";
    result += "    // diffuse shading\n";
    result += "    float diffuseFactor = max(dot(normalize(normal), ligthDirectionWorldSpace), 0.0);\n";
    result += "    // specular shading\n";
    result += "    vec3 reflectDirectionWorldSpace = reflect(-ligthDirectionWorldSpace, normalize(normal));\n";
    result += "    float specularFactor = pow(max(dot(cameraDirectionWorldSpace, reflectDirectionWorldSpace), 0.0), shininess);\n";
    result += "    // attenuation\n";
    result += "    float distance = length(light.position - positionWorldSpace);\n";
    result += "    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));\n";
    result += "    // combine results\n";
    result += "    vec3 ambient = light.ambient * ambientColor;\n";
    result += "    vec3 diffuse = light.diffuse * diffuseFactor * diffuseColor;\n";
    result += "    vec3 specular = light.specular * specularFactor * specularColor;\n";
    result += "    ambient *= attenuation;\n";
    result += "    diffuse *= attenuation;\n";
    result += "    specular *= attenuation;\n\n";
    result += "    return (ambient + diffuse + specular);\n";
    result += "}\n";
}

void AppendFunctionLightSpot(std::string & result)
{
    result += "vec3 CalcLightSpot(LightSpot light, vec3 ambientColor, vec3 diffuseColor, vec3 specularColor, vec3 cameraDirectionWorldSpace, vec3 normal)\n";
    result += "{\n";
    result += "    vec3 ligthDirectionWorldSpace = normalize(light.position - positionWorldSpace);\n";
    result += "    // diffuse shading\n";
    result += "    float diffuseFactor = max(dot(normal, ligthDirectionWorldSpace), 0.0);\n";
    result += "    // specular shading\n";
    result += "    vec3 reflectDirectionWorldSpace = reflect(-ligthDirectionWorldSpace, normal);\n";
    result += "    float specularFactor = pow(max(dot(cameraDirectionWorldSpace, reflectDirectionWorldSpace), 0.0), shininess);\n";
    result += "    // attenuation\n";
    result += "    float distance = length(light.position - positionWorldSpace);\n";
    result += "    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));\n";
    result += "    // spotlight intensity\n";
    result += "    float theta = dot(ligthDirectionWorldSpace, normalize(-light.direction));\n";
    result += "    float epsilon = light.cutOff - light.outerCutOff;\n";
    result += "    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);\n";
    result += "    // combine results\n";
    result += "    vec3 ambient = light.ambient * ambientColor;\n";
    result += "    vec3 diffuse = light.diffuse * diffuseFactor * diffuseColor;\n";
    result += "    vec3 specular = light.specular * specularFactor * specularColor;\n";
    result += "    ambient *= attenuation * intensity;;\n";
    result += "    diffuse *= attenuation * intensity;\n";
    result += "    specular *= attenuation * intensity;\n\n";
    result += "    return (ambient + diffuse + specular);\n";
    result += "}\n";
}

void AppendFunctionLight(const ModelShader::Config & config, std::string & result)
{
    result += "// *** light functions ***\n";
    if (config.lightDirection)
        AppendFunctionLightDirectional(result);
    if (config.lightPointCount)
        AppendFunctionLightPoint(result);
    if (config.lightSpotCount)
        AppendFunctionLightSpot(result);
    result += "// ***********************\n";
}

void AppendFunctionColor(const ModelShader::Config & config, std::string & result)
{
    result += "// *** color functions ***\n";
    result += "vec3 CalcAmbientColor()\n";
    result += "{\n";
    if (config.textureAmbientCount)
        result += "    return texture2D(textureAmbient[0], vertexUVA).rgb;\n";
    else
        result += "    return material.ambient;\n";
    result += "}\n";
    result += "vec3 CalcDiffuseColor()\n";
    result += "{\n";
    if (config.textureDiffuseCount)
        result += "    return texture2D(textureDiffuse[0], vertexUVA).rgb;\n";
    else
        result += "    return material.diffuse;\n";
    result += "}\n";
    result += "vec3 CalcSpecularColor()\n";
    result += "{\n";
    if (config.textureSpecularCount)
        result += "    return texture2D(textureDiffuse[0], vertexUVA).rgb;\n";
    else
        result += "    return material.specular;\n";
    result += "}\n";
    result += "// ***********************\n";
}

void AppendBodyFragment(const ModelShader::Config & config, std::string & result)
{
    result += "// *** fragment body ***\n";
    result += "void main(void)\n";
    result += "{\n";
    result += "    vec3 ambientColor = CalcAmbientColor();\n";
    result += "    vec3 diffuseColor = CalcDiffuseColor();\n";
    result += "    vec3 specularColor = CalcSpecularColor();\n\n";

    result += "    vec3 cameraDirectionWorldSpace = normalize(cameraWorldSpace - positionWorldSpace);\n\n";
 
    result += "    vec3 result = vec3(0.0, 0.0, 0.0);\n";
    result += "    vec3 normal = vec3(0.0, 0.0, 0.0);\n\n";
    if (config.textureNormalCount)
    {
        result += "    vec3 normalTangentSpace = texture2D(textureNormal[0], vertexUVA).rgb;\n";
        result += "    normalTangentSpace = normalize(normalTangentSpace * 2.0 - 1.0);\n";
        result += "    normal = normalize(TBN * normalTangentSpace);\n\n";
    }
    else
    {
        result += "    normal = normalize(normalWorldSpace);\n\n";
    }
    if (config.lightDirection)
        result += "    result = CalcLightDirectional(lightDirectional, ambientColor, diffuseColor, specularColor, cameraDirectionWorldSpace, normal);\n\n";
    if (config.lightPointCount)
    {
        result += "    for(int i = 0; i < " + std::to_string(config.lightPointCount) + "; i++)\n";
        result += "        result += CalcLightPoint(lightPoint[i], ambientColor, diffuseColor, specularColor, cameraDirectionWorldSpace, normal);\n\n";
    }
    if (config.lightSpotCount)
    {
        result += "    for(int i = 0; i < " + std::to_string(config.lightSpotCount) + "; i++)\n";
        result += "        result += CalcLightSpot(lightSpot[i], ambientColor, diffuseColor, specularColor, cameraDirectionWorldSpace, normal);\n\n";
    }
    result += "    gl_FragColor.rgb = result;\n";
    result += "    gl_FragColor.a = 1.0;\n";
    result += "}\n";
    result += "// *********************\n";
}

std::string GenerateVertex(const ModelShader::Config & config)
{
    std::string result;

    AppendHeader(result);
    AppendAttributes(config, result);
    AppendUniforms(config, result);
    AppendVaryings(config, result);
    AppendFunctionSupport(result);
    AppendBodyVertex(config, result);

    return result;
}

std::string GenerateFragment(const ModelShader::Config & config)
{
    std::string result;

    AppendHeader(result);
    AppendUniforms(config, result);
    AppendUniformsMaterial(config, result);
    AppendUniformsLight(config, result);
    AppendVaryings(config, result);
    AppendFunctionLight(config, result);
    AppendFunctionColor(config, result);
    AppendBodyFragment(config, result);

    return result;
}

ShaderGenerator::Result ShaderGenerator::Generate(const ModelShader::Config & config)
{
    return { GenerateVertex(config), GenerateFragment(config) };
}
