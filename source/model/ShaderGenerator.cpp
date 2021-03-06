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
    for (uint32_t i = 0; i < config.GetUVChannelsCount(); ++i)
        result += "attribute vec2 vertexUV" + std::to_string(i) + ";\n";
    if (config.textures.normal.size())
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
    if (config.flags & (uint32_t)ModelShader::Config::Flags::UseRuntimeMaterial)
    {
        result += "struct Material\n";
        result += "{\n";
        result += "    vec3 ambient;\n";
        result += "    vec3 diffuse;\n";
        result += "    vec3 specular;\n";
        result += "    float shininess;\n";
        result += "    float shininessStrength;\n";
        result += "};\n";
        result += "uniform Material material;\n";
    }
    if (config.textures.ambient.size())
        result += "uniform sampler2D textureAmbient[" + std::to_string(config.textures.ambient.size()) + "];\n";
    if (config.textures.diffuse.size())
        result += "uniform sampler2D textureDiffuse[" + std::to_string(config.textures.diffuse.size()) + "];\n";
    if (config.textures.specular.size())
        result += "uniform sampler2D textureSpecular[" + std::to_string(config.textures.specular.size()) + "];\n";
    if (config.textures.normal.size())
        result += "uniform sampler2D textureNormal[" + std::to_string(config.textures.normal.size()) + "];\n";
    if (config.textures.lightmap.size())
        result += "uniform sampler2D textureLightmap[" + std::to_string(config.textures.lightmap.size()) + "];\n";
    result += "// *************************\n";
}

void AppendUniformsLight(const ModelShader::Config & config, std::string & result)
{
    result += "// *** light uniforms ***\n";
    if (config.light.directional)
    {
        result += "struct LightDirectional\n";
        result += "{\n";
        result += "    vec3 direction;\n";
        result += "    vec3 ambient;\n";
        result += "    vec3 diffuse;\n";
        result += "    vec3 specular;\n";
        // begin shadow specific
        result += "    mat4 lightSpaceMatrix;\n";
        result += "    vec2 depthMapSize;\n";
        result += "    vec2 shadowParams;\n";
        // end shadow specific
        result += "};\n";
        result += "uniform LightDirectional lightDirectional;\n";
        result += "uniform sampler2D lightDirectionalDepthMap;\n";
    }
    if (config.light.pointCount)
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
        // begin shadow specific
        result += "    mat4 lightSpaceMatrix;\n";
        result += "    vec2 depthMapSize;\n";
        result += "    float farPlane;\n";
        // end shadow specific
        result += "};\n";
        result += "uniform LightPoint lightPoint[" + std::to_string(config.light.pointCount) + "];\n";
        result += "uniform samplerCube lightPointDepthMaps[" + std::to_string(config.light.pointCount) + "];\n";
    }
    if (config.light.spotCount)
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
        // begin shadow specific
        result += "    mat4 lightSpaceMatrix;\n";
        result += "    vec2 depthMapSize;\n";
        // end shadow specific
        result += "};\n";
        result += "uniform LightSpot lightSpot[" + std::to_string(config.light.spotCount) + "];\n";
        result += "uniform sampler2D lightSpotDepthMaps[" + std::to_string(config.light.spotCount) + "];\n";
    }
    result += "// **********************\n";
}

void AppendVaryings(const ModelShader::Config & config, std::string & result)
{
    result += "// *** varyings ***\n";
    result += "varying vec3 positionWorldSpace;\n";
    if (config.textures.normal.size())
        result += "varying mat3 TBN;\n";
    else
        result += "varying vec3 normalWorldSpace;\n";
    if (config.GetUVChannelsCount())
        result += "varying vec2 vertexUVA[" + std::to_string(config.GetUVChannelsCount()) + "];\n";

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
    if (config.textures.normal.size())
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
    for (uint32_t i = 0; i < config.GetUVChannelsCount(); ++i)
        result += "  vertexUVA[" + std::to_string(i) + "] = vertexUV" + std::to_string(i) + ";\n";

    result += "}\n";
    result += "// ******************\n";
}

void AppendFunctionLightCommon(std::string & result, const std::string & shininess, ModelShader::ShadingModel model)
{
    result += "    // diffuse shading\n";
    result += "    float diffuseFactor = max(dot(normalize(normal), ligthDirectionWorldSpace), 0.0);\n";
    result += "    // specular shading\n";
    if (model == ModelShader::ShadingModel::Phong)
    {
        result += "    vec3 reflectDirectionWorldSpace = reflect(-ligthDirectionWorldSpace, normalize(normal));\n";
        result += "    float specularFactor = pow(max(dot(cameraDirectionWorldSpace, reflectDirectionWorldSpace), 0.0), " + shininess + ");\n";
    }
    else if (model == ModelShader::ShadingModel::BlinnPhong)
    {
        result += "    vec3 halfwayDirectionWorldSpace = normalize(ligthDirectionWorldSpace + cameraDirectionWorldSpace);\n";
        result += "    float specularFactor = pow(max(dot(normal, halfwayDirectionWorldSpace), 0.0), " + shininess + ");\n";
    }
    result += "    // combine results\n";
    result += "    vec3 ambient = light.ambient * ambientColor;\n";
    result += "    vec3 diffuse = light.diffuse * diffuseFactor * diffuseColor;\n";
    result += "    vec3 specular = light.specular * specularFactor * specularColor;\n\n";
}

void AppendFunctionLightDirectional(std::string & result, const std::string & shininess, ModelShader::ShadingModel model)
{
    result += "vec3 CalcLightDirectional(LightDirectional light, vec3 ambientColor, vec3 diffuseColor, vec3 specularColor, vec3 cameraDirectionWorldSpace, vec3 normal, float shadow)\n";
    result += "{\n";
    result += "    vec3 ligthDirectionWorldSpace = normalize(-light.direction);\n";
    AppendFunctionLightCommon(result, shininess, model);
    result += "    return ambient + (diffuse + specular)*(1.0 - shadow);\n";
    result += "}\n";
}

void AppendFunctionLightPoint(std::string & result, const std::string & shininess, ModelShader::ShadingModel model)
{
    result += "vec3 CalcLightPoint(LightPoint light, vec3 ambientColor, vec3 diffuseColor, vec3 specularColor, vec3 cameraDirectionWorldSpace, vec3 normal, float shadow)\n";
    result += "{\n";
    result += "    vec3 ligthDirectionWorldSpace = normalize(light.position - positionWorldSpace);\n";
    AppendFunctionLightCommon(result, shininess, model);
    result += "    // attenuation\n";
    result += "    float distance = length(light.position - positionWorldSpace);\n";
    result += "    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));\n\n";
    result += "    ambient *= attenuation;\n";
    result += "    diffuse *= attenuation;\n";
    result += "    specular *= attenuation;\n\n";
    result += "    return ambient + (diffuse + specular)*(1.0 - shadow);\n";
    result += "}\n";
}

void AppendFunctionLightSpot(std::string & result, const std::string & shininess, ModelShader::ShadingModel model)
{
    result += "vec3 CalcLightSpot(LightSpot light, vec3 ambientColor, vec3 diffuseColor, vec3 specularColor, vec3 cameraDirectionWorldSpace, vec3 normal, float shadow)\n";
    result += "{\n";
    result += "    vec3 ligthDirectionWorldSpace = normalize(light.position - positionWorldSpace);\n";
    AppendFunctionLightCommon(result, shininess, model);
    result += "    // attenuation\n";
    result += "    float distance = length(light.position - positionWorldSpace);\n";
    result += "    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));\n\n";
    result += "    // spotlight intensity\n";
    result += "    float theta = dot(ligthDirectionWorldSpace, normalize(-light.direction));\n";
    result += "    float epsilon = light.cutOff - light.outerCutOff;\n";
    result += "    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);\n\n";
    result += "    ambient *= attenuation * intensity;\n";
    result += "    diffuse *= attenuation * intensity;\n";
    result += "    specular *= attenuation * intensity;\n\n";
    result += "    return ambient + (diffuse + specular)*(1.0 - shadow);\n";
    result += "}\n";
}

void AppendFunctionLight(const ModelShader::Config & config, std::string & result)
{
    std::string shininess = std::to_string(config.material.shininess);
    if (config.flags & (uint32_t)ModelShader::Config::Flags::UseRuntimeMaterial)
        shininess = "material.shininess";

    result += "// *** light functions ***\n";
    if (config.light.directional)
        AppendFunctionLightDirectional(result, shininess, config.shading);
    if (config.light.pointCount)
        AppendFunctionLightPoint(result, shininess, config.shading);
    if (config.light.spotCount)
        AppendFunctionLightSpot(result, shininess, config.shading);
    result += "// ***********************\n";
}

void AppendFunctionShadowDirectional(const ModelShader::Config & config, std::string & result)
{
    result += "float ShadowCalculationDirectional(LightDirectional light, in sampler2D depthMap, vec3 normal)\n";
    result += "{\n";
    result += "    vec4 position = light.lightSpaceMatrix * vec4(positionWorldSpace, 1.0);\n";
    result += "    vec3 lightDirection = normalize(light.direction);\n";
    result += "    // perform perspective divide, result is in [-1, 1]\n";
    result += "    vec3 projCoords = position.xyz / position.w;\n";
    result += "    // tansform to range [0, 1], this is screen coordinates for sampling in depth map\n";
    result += "    projCoords = projCoords * 0.5 + 0.5;\n";
    result += "    // get closest depth value from light's perspective (using [0,1] range positionLightSpace as coords)\n";
    result += "    float closestDepth = texture2D(depthMap, projCoords.xy).r;\n";
    result += "    // get depth of current fragment from light's perspective\n";
    result += "    float currentDepth = projCoords.z;\n";
    result += "    // check whether current frag pos is in shadow\n";
    result += "    // bias to prevent shadow acne\n";
    result += "    float bias = max(light.shadowParams.x * (1.0 - dot(normal, lightDirection)), light.shadowParams.y);\n";
    //result += "    float bias = 0.001;\n";
    result += "    // check whether current frag pos is in shadow\n";
    //result += "    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.1;\n";
    result += "    // PCF\n";
    result += "    float shadow = 0.0;\n";
    result += "    vec2 texelSize = 1.0 / light.depthMapSize;\n";
    result += "    for(int x = -1; x <= 1; ++x)\n";
    result += "    {\n";
    result += "        for(int y = -1; y <= 1; ++y)\n";
    result += "        {\n";
    result += "            float pcfDepth = texture2D(depthMap, projCoords.xy + vec2(x, y) * texelSize).r;\n";
    result += "            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;\n";
    result += "        }\n";
    result += "    }\n";
    result += "    shadow /= 9.0;\n";
    result += "    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.\n";
    result += "    if(projCoords.z > 1.0)\n";
    result += "        shadow = 0.0;\n";
    result += "    return shadow;\n";
    result += "}\n";
}

void AppendFunctionShadowPoint(const ModelShader::Config & config, std::string & result)
{
    //result += "vec3 SAMPLE_OFFSET_DIRECTIONS[20] = vec3[](\n";
    ////result += "(\n";
    //result += "   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1),\n";
    //result += "   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),\n";
    //result += "   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),\n";
    //result += "   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),\n";
    //result += "   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)\n";
    //result += ");\n";
    result += "vec3 SAMPLE_OFFSET_DIRECTIONS(in int i)\n";
    result += "{\n";
    result += "    if (i == 0)       return vec3( 1,  1,  1);\n";
    result += "    else if (i == 1)  return vec3( 1, -1,  1);\n";
    result += "    else if (i == 2)  return vec3(-1, -1,  1);\n";
    result += "    else if (i == 3)  return vec3(-1,  1,  1);\n";
    result += "    else if (i == 4)  return vec3( 1,  1, -1);\n";
    result += "    else if (i == 5)  return vec3( 1, -1, -1);\n";
    result += "    else if (i == 6)  return vec3(-1, -1, -1);\n";
    result += "    else if (i == 7)  return vec3(-1,  1, -1);\n";
    result += "    else if (i == 8)  return vec3( 1,  1,  0);\n";
    result += "    else if (i == 9)  return vec3( 1, -1,  0);\n";
    result += "    else if (i == 10) return vec3(-1, -1,  0);\n";
    result += "    else if (i == 11) return vec3(-1,  1,  0);\n";
    result += "    else if (i == 12) return vec3( 1,  0,  1);\n";
    result += "    else if (i == 13) return vec3(-1,  0,  1);\n";
    result += "    else if (i == 14) return vec3( 1,  0, -1);\n";
    result += "    else if (i == 15) return vec3(-1,  0, -1);\n";
    result += "    else if (i == 16) return vec3( 0,  1,  1);\n";
    result += "    else if (i == 17) return vec3( 0, -1,  1);\n";
    result += "    else if (i == 18) return vec3( 0, -1, -1);\n";
    result += "    else if (i == 19) return vec3( 0,  1, -1);\n";
    result += "    return vec3(1, 1, 1);\n";
    result += "}\n";

    result += "\n";
    result += "float ApplyCubePCF(in samplerCube depthMap, float farPlane, float currentDepth, vec3 fragmentToLightWorldSpace)\n";
    result += "{\n";
    result += "    float shadow = 0.0;\n";
    result += "    float bias   = 0.15;\n";
    result += "    const int samples  = 20;\n";
    result += "    float viewDistance = length(cameraWorldSpace - positionWorldSpace);\n";
    result += "    float diskRadius = (1.0 + (viewDistance / farPlane)) / 25.0;\n";
    result += "\n";
    result += "    for(int i = 0; i < samples; ++i)\n";
    result += "    {\n";
    //result += "        float closestDepth = textureCube(depthMap, fragmentToLightWorldSpace + SAMPLE_OFFSET_DIRECTIONS[i] * diskRadius).r;\n";
    result += "        float closestDepth = textureCube(depthMap, fragmentToLightWorldSpace + SAMPLE_OFFSET_DIRECTIONS(i) * diskRadius).r;\n";
    result += "        closestDepth *= farPlane; // Undo mapping [0;1]\n";
    result += "        if(currentDepth - bias > closestDepth)\n";
    result += "            shadow += 1.0;\n";
    result += "    }\n";
    result += "\n";
    result += "    shadow /= float(samples);\n";
    result += "\n";
    result += "    return shadow;\n";
    result += "}\n";
    result += "\n";
    result += "float ShadowCalculationPoint(LightPoint light, in samplerCube depthMap)\n";
    result += "{\n";
    result += "    // get vector from fragment to light\n";
    result += "    vec3 fragmentToLightWorldSpace = positionWorldSpace - light.position;\n";
    result += "    // use the light to fragment vector to sample from the depth map\n";
    result += "    float closestDepth = textureCube(depthMap, fragmentToLightWorldSpace).r;\n";
    result += "    // it is currently in linear range between [0,1]. Re-transform back to original value\n";
    result += "    closestDepth *= light.farPlane;\n";
    result += "    // now get current linear depth as the length between the fragment and light position\n";
    result += "    float currentDepth = length(fragmentToLightWorldSpace);\n";
    result += "    // now test for shadows\n";
    result += "    //float bias = 0.05;\n";
    result += "    //float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;\n";
    result += "    float shadow = ApplyCubePCF(depthMap, light.farPlane, currentDepth, fragmentToLightWorldSpace);\n";
    result += "\n";
    result += "    return shadow;\n";
    result += "    //return closestDepth;\n";
    result += "}\n";
}

void AppendFunctionShadowSpot(const ModelShader::Config & config, std::string & result)
{
    result += "float ShadowCalculationSpot(LightSpot light, in sampler2D depthMap, vec3 normal)\n";
    result += "{\n";
    result += "    vec4 position = light.lightSpaceMatrix * vec4(positionWorldSpace, 1.0);\n";
    result += "    vec3 projCoords = position.xyz / position.w;\n";
    result += "    projCoords = projCoords * 0.5 + 0.5;\n";
    result += "    float closestDepth = texture2D(depthMap, projCoords.xy).r;\n";
    result += "    float currentDepth = projCoords.z;\n";
    result += "    float bias = 0.001;\n";
    result += "    // PCF\n";
    result += "    float shadow = 0.0;\n";
    result += "    vec2 texelSize = 1.0 / light.depthMapSize;\n";
    result += "    for(int x = -1; x <= 1; ++x)\n";
    result += "    {\n";
    result += "        for(int y = -1; y <= 1; ++y)\n";
    result += "        {\n";
    result += "            float pcfDepth = texture2D(depthMap, projCoords.xy + vec2(x, y) * texelSize).r;\n";
    result += "            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;\n";
    result += "        }\n";
    result += "    }\n";
    result += "    shadow /= 9.0;\n";
    result += "    if(projCoords.z > 1.0)\n";
    result += "        shadow = 0.0;\n";
    result += "    return shadow;\n";
    result += "}\n";
}

void AppendFunctionShadow(const ModelShader::Config & config, std::string & result)
{
    result += "// *** shadow functions ***\n";
    if (config.light.directional)
        AppendFunctionShadowDirectional(config, result);
    if (config.light.pointCount)
        AppendFunctionShadowPoint(config, result);
    if (config.light.spotCount)
        AppendFunctionShadowSpot(config, result);
    result += "// ***********************\n";
}

uint32_t GetTotalLightCount(const ModelShader::Config & config)
{
    return config.light.pointCount + config.light.spotCount + (config.light.directional ? 1 : 0);
}

void AppendFunctionColorOperations(std::string & result)
{
    result += "// *** color operations ***\n";
    result += "vec3 CalcColorAdd(vec3 c1, vec3 c2)\n";
    result += "{\n";
    result += "    return c1 + c2;\n";
    result += "}\n";
    result += "vec3 CalcColorMultiply(vec3 c1, vec3 c2)\n";
    result += "{\n";
    result += "    return c1 * c2;\n";
    result += "}\n";
    result += "vec3 CalcColorSubstract(vec3 c1, vec3 c2)\n";
    result += "{\n";
    result += "    return c1 - c2;\n";
    result += "}\n";
    result += "vec3 CalcColorDivide(vec3 c1, vec3 c2)\n";
    result += "{\n";
    result += "    return c1 / c2;\n";
    result += "}\n";
    result += "vec3 CalcColorSmoothAdd(vec3 c1, vec3 c2)\n";
    result += "{\n";
    result += "    return (c1 + c2) - (c1 * c2);\n";
    result += "}\n";
    result += "vec3 CalcColorSignedAdd(vec3 c1, vec3 c2)\n";
    result += "{\n";
    result += "    return c1 + (c2 - 0.5);\n";
    result += "}\n";
    result += "// ************************\n";
}

std::string MapOperation(ModelShader::TextureStackEntry::Operation operation)
{
    switch (operation)
    {
    case ModelShader::TextureStackEntry::Operation::Add:
        return "CalcColorAdd";
    case ModelShader::TextureStackEntry::Operation::Divide:
        return "CalcColorAdd";
    case ModelShader::TextureStackEntry::Operation::Multiply:
        return "CalcColorMultiply";
    case ModelShader::TextureStackEntry::Operation::SignedAdd:
        return "CalcColorSignedAdd";
    case ModelShader::TextureStackEntry::Operation::SmoothAdd:
        return "CalcColorSmoothAdd";
    case ModelShader::TextureStackEntry::Operation::Substract:
        return "CalcColorSubstract";
    }
    return "CalcColorMultiply";
}

void AppendFunctionColorTextureStack(std::string name, const std::string & base, const std::vector<ModelShader::TextureStackEntry> & stack, std::string & result)
{
    result += "vec3 Calc" + name + "Color()\n";
    result += "{\n";
    result += "    vec3 result = " + base + ";\n\n";
    
    for (size_t i = 0; i < stack.size(); ++i)
    {
        std::string sample("texture2D(texture" + name + "[" + std::to_string(i) + "], vertexUVA[" + std::to_string(stack[i].uvIndex) + "]).rgb");
        result += "    result = " + MapOperation(stack[i].operation) + "(result, " + sample + ");\n";
    }

    result += "    return result;\n";
    result += "}\n";
}

static std::string Convert(const glm::vec3 & c)
{
    return "vec3(" + std::to_string(c.r) + ", " + std::to_string(c.g) + ", " + std::to_string(c.b) + ")";
}

void AppendFunctionColor(const ModelShader::Config & config, std::string & result)
{
    bool useMaterial = config.flags & (uint32_t)ModelShader::Config::Flags::UseRuntimeMaterial;

    result += "// *** color functions ***\n";
    AppendFunctionColorTextureStack("Ambient", useMaterial ? "material.ambient" : Convert(config.material.ambient), config.textures.ambient, result);
    AppendFunctionColorTextureStack("Diffuse", useMaterial ? "material.diffuse" : Convert(config.material.diffuse), config.textures.diffuse, result);
    AppendFunctionColorTextureStack("Specular", useMaterial ? "material.specular" : Convert(config.material.specular), config.textures.specular, result);
    // 
    if (config.textures.lightmap.size())
        AppendFunctionColorTextureStack("Lightmap", "vec3(1.0, 1.0, 1.0)", config.textures.lightmap, result);
    if (config.textures.normal.size())
        AppendFunctionColorTextureStack("Normal", "vec3(1.0, 1.0, 1.0)", config.textures.normal, result);

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

    if (config.flags & (uint32_t)ModelShader::Config::Flags::UseRuntimeMaterial)
        result += "    specularColor *= material.shininessStrength;\n\n";
    else
        result += "    specularColor *= " + std::to_string(config.material.shininessStrength) + ";\n\n";

    result += "    vec3 cameraDirectionWorldSpace = normalize(cameraWorldSpace - positionWorldSpace);\n\n";
 
    result += "    vec3 result = vec3(0.0, 0.0, 0.0);\n";
    result += "    vec3 normal = vec3(0.0, 0.0, 0.0);\n\n";
    if (config.textures.normal.size())
    {
        result += "    vec3 normalTangentSpace = CalcNormalColor();\n";
        result += "    normalTangentSpace = normalize(normalTangentSpace * 2.0 - 1.0);\n";
        result += "    normal = normalize(TBN * normalTangentSpace);\n\n";
    }
    else
    {
        result += "    normal = normalize(normalWorldSpace);\n\n";
    }

    // shadow calculation
    result += "    float shadow = 0.0;\n\n";
    if (config.light.directional)
        result += "    shadow += ShadowCalculationDirectional(lightDirectional, lightDirectionalDepthMap, normal);\n\n";
    if (config.light.pointCount)
    {
        result += "    for(int i = 0; i < " + std::to_string(config.light.pointCount) + "; i++)\n";
        result += "        shadow += ShadowCalculationPoint(lightPoint[i], lightPointDepthMaps[i]);\n";
    }
    if (config.light.spotCount)
    {
        result += "    for(int i = 0; i < " + std::to_string(config.light.spotCount) + "; i++)\n";
        result += "        shadow += ShadowCalculationSpot(lightSpot[i], lightSpotDepthMaps[i], normal);\n";
    }
    if (GetTotalLightCount(config))
        result += "    shadow /= " + std::to_string(GetTotalLightCount(config)) + ".0;\n\n";

    // light calculations
    if (config.light.directional)
        result += "    result = CalcLightDirectional(lightDirectional, ambientColor, diffuseColor, specularColor, cameraDirectionWorldSpace, normal, shadow);\n\n";
    if (config.light.pointCount)
    {
        result += "    for(int i = 0; i < " + std::to_string(config.light.pointCount) + "; i++)\n";
        result += "        result += CalcLightPoint(lightPoint[i], ambientColor, diffuseColor, specularColor, cameraDirectionWorldSpace, normal, shadow);\n\n";
    }
    if (config.light.spotCount)
    {
        result += "    for(int i = 0; i < " + std::to_string(config.light.spotCount) + "; i++)\n";
        result += "        result += CalcLightSpot(lightSpot[i], ambientColor, diffuseColor, specularColor, cameraDirectionWorldSpace, normal, shadow);\n\n";
    }

    // final color is changed by lightmap
    //if (config.textures.lightmap.size())
    //    result += "    result *= CalcLightmapColor();\n\n";

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
    AppendFunctionShadow(config, result);
    AppendFunctionColorOperations(result);
    AppendFunctionColor(config, result);
    AppendBodyFragment(config, result);

    return result;
}

ShaderGenerator::Result ShaderGenerator::Generate(const ModelShader::Config & config)
{
    return { GenerateVertex(config), GenerateFragment(config) };
}
