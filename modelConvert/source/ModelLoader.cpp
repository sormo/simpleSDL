#include "ModelLoader.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/DefaultLogger.hpp>

#include <string>
#include <iostream>
#include <vector>

ModelData::TextureOperation MapOperation(aiTextureOp operation)
{
    switch (operation)
    {
    case aiTextureOp_Multiply:
        return ModelData::TextureOperation_Multiply;
    case aiTextureOp_Add:
        return ModelData::TextureOperation_Add;
    case aiTextureOp_Subtract:
        return ModelData::TextureOperation_Substract;
    case aiTextureOp_Divide:
        return ModelData::TextureOperation_Divide;
    case aiTextureOp_SmoothAdd:
        return ModelData::TextureOperation_SmoothAdd;
    case aiTextureOp_SignedAdd:
        return ModelData::TextureOperation_SignedAdd;
    }

    return ModelData::TextureOperation_MAX;
}

ModelData::TextureMapMode MapMapMode(aiTextureMapMode mapping)
{
    switch (mapping)
    {
    case aiTextureMapMode_Wrap:
        return ModelData::TextureMapMode_Wrap;
    case aiTextureMapMode_Clamp:
        return ModelData::TextureMapMode_Clamp;
    case aiTextureMapMode_Decal:
        return ModelData::TextureMapMode_Decal;
    case aiTextureMapMode_Mirror:
        return ModelData::TextureMapMode_Mirror;
    }

    return ModelData::TextureMapMode_MAX;
}

// checks all material textures of a given type and loads the textures if they're not loaded yet.
// the required info is returned as a Texture struct.
std::vector<std::unique_ptr<ModelData::TextureT>> ProcessTexture(aiMaterial * material, aiTextureType textureType)
{
    std::vector<std::unique_ptr<ModelData::TextureT>> result;

    for (uint32_t i = 0; i < material->GetTextureCount(textureType); i++)
    {
        aiString texturePath;
        uint32_t uvIndex = 0;
        ai_real blendFactor = 1.0f;
        aiTextureOp operation;
        aiTextureMapMode mapMode[2];

        if (material->GetTexture(textureType, i, &texturePath, nullptr, &uvIndex, &blendFactor, &operation, mapMode) != AI_SUCCESS)
        {
            std::cout << "Error retrieving texture!!\n";
            continue;
        }

        std::unique_ptr<ModelData::TextureT> texture = std::make_unique<ModelData::TextureT>();

        texture->path = texturePath.C_Str();
        texture->uvIndex = uvIndex;
        texture->blendFactor = blendFactor;
        texture->operation = MapOperation(operation);
        texture->mappingU = MapMapMode(mapMode[0]);
        texture->mappingV = MapMapMode(mapMode[1]);

        result.push_back(std::move(texture));
    }

    return result;
}

std::unique_ptr<ModelData::MaterialT> ProcessMaterial(aiMaterial * material)
{
    std::unique_ptr<ModelData::MaterialT> result = std::make_unique<ModelData::MaterialT>();

    aiColor4D ambientColor;
    if (material->Get(AI_MATKEY_COLOR_AMBIENT, ambientColor) == AI_SUCCESS)
    {
        result->ambient = std::make_unique<ModelData::Color>(ambientColor.r, ambientColor.g, ambientColor.b, ambientColor.a);
    }
    aiColor4D diffuseColor;
    if (material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor) == AI_SUCCESS)
    {
        result->diffuse = std::make_unique<ModelData::Color>(diffuseColor.r, diffuseColor.g, diffuseColor.b, diffuseColor.a);
    }
    aiColor4D specularColor;
    if (material->Get(AI_MATKEY_COLOR_SPECULAR, specularColor) == AI_SUCCESS)
    {
        result->specular = std::make_unique<ModelData::Color>(specularColor.r, specularColor.g, specularColor.b, specularColor.a);
    }
    float shininess;
    if (material->Get(AI_MATKEY_SHININESS, shininess) == AI_SUCCESS)
    {
        result->shininess = shininess;
    }
    float shininessStrength;
    if (material->Get(AI_MATKEY_SHININESS_STRENGTH, shininessStrength) == AI_SUCCESS)
    {
        result->shininessStrength = shininessStrength;
    }

    auto processTexture = [](aiMaterial * material, aiTextureType textureType, std::vector<std::unique_ptr<ModelData::TextureT>> & result)
    {
        auto processed = ProcessTexture(material, textureType);
        result.insert(result.end(), std::make_move_iterator(processed.begin()), std::make_move_iterator(processed.end()));
    };

    // ambient maps
    processTexture(material, aiTextureType_AMBIENT, result->textureAmbient);
    // diffuse maps
    processTexture(material, aiTextureType_DIFFUSE, result->textureDiffuse);
    //specular maps
    processTexture(material, aiTextureType_SPECULAR, result->textureSpecular);
    // normal maps
    processTexture(material, aiTextureType_NORMALS, result->textureNormal);
    // height maps
    processTexture(material, aiTextureType_HEIGHT, result->textureHeight);
    // lightmap maps
    processTexture(material, aiTextureType_LIGHTMAP, result->textureLightmap);
    // opacity maps
    processTexture(material, aiTextureType_OPACITY, result->textureOpacity);
    // emissive maps
    processTexture(material, aiTextureType_EMISSIVE, result->textureEmissive);
    // shininess maps
    processTexture(material, aiTextureType_SHININESS, result->textureShininess);

    return result;
}

std::unique_ptr<ModelData::MeshT> ProcessMesh(aiMesh * mesh)
{
    // data to fill
    std::unique_ptr<ModelData::MeshT> result = std::make_unique<ModelData::MeshT>();

    // Walk through each of the mesh's vertices
    for (uint32_t i = 0; i < mesh->mNumVertices; i++)
    {
        if (mesh->HasPositions())
            result->positions.push_back({ mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z });
        if (mesh->HasNormals())
            result->normals.push_back({ mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z });
        
        if (mesh->HasTangentsAndBitangents())
        {
            result->tangents.push_back({ mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z });
            result->bitangents.push_back({ mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z });
        }
    }

    // Store UV channels as continuous array
    {
        uint32_t index = 0;
        while (mesh->HasTextureCoords(index))
        {
            for (uint32_t i = 0; i < mesh->mNumVertices; i++)
                result->texCoords.push_back({ mesh->mTextureCoords[index][i].x, mesh->mTextureCoords[index][i].y });
            index++;
        }
    }

    // Store colors as continuous array
    {
        uint32_t index = 0;
        while (mesh->HasVertexColors(index))
        {
            for (uint32_t i = 0; i < mesh->mNumVertices; i++)
                result->colors.push_back({ mesh->mColors[index][i].r, mesh->mColors[index][i].g, mesh->mColors[index][i].b, mesh->mColors[index][i].a });
            index++;
        }
    }

    // now walk through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
    for (uint32_t i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        // retrieve all indices of the face and store them in the indices vector
        for (uint32_t j = 0; j < face.mNumIndices; j++)
            result->indices.push_back(face.mIndices[j]);
    }

    result->material = mesh->mMaterialIndex;

    // return a mesh object created from the extracted mesh data
    return result;
}

std::unique_ptr<ModelData::Mat4> Convert(const aiMatrix4x4 & m)
{
    return std::make_unique<ModelData::Mat4>(
        m.a1, m.a2, m.a3, m.a4,
        m.b1, m.b2, m.b3, m.b4,
        m.c1, m.c2, m.c3, m.c4,
        m.d1, m.d2, m.d3, m.d4);
}

std::unique_ptr<ModelData::TreeT> ProcessTree(aiNode * node)
{
    std::unique_ptr<ModelData::TreeT> result = std::make_unique<ModelData::TreeT>();

    result->transform = Convert(node->mTransformation);
    for (uint32_t i = 0; i < node->mNumMeshes; ++i)
        result->meshes.push_back(node->mMeshes[i]);

    for (uint32_t i = 0; i < node->mNumChildren; ++i)
        result->childs.push_back(ProcessTree(node->mChildren[i]));

    return result;
}

void CreateLogger()
{
    Assimp::DefaultLogger::create(nullptr, Assimp::Logger::NORMAL, aiDefaultLogStream_STDOUT);
}

void DestroyLogger()
{
    // Kill it after the work is done
    Assimp::DefaultLogger::kill();
}

std::optional<ModelData::ModelT> LoadModel(const char * path)
{
    CreateLogger();

    // read file via ASSIMP
    Assimp::Importer importer;

    // TODO for now exclude lines and points
    importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_POINT | aiPrimitiveType_LINE);

    const aiScene * scene = importer.ReadFile(path, aiProcessPreset_TargetRealtime_MaxQuality);
    
    DestroyLogger();
    
    // check for errors
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "Error assimp: " << importer.GetErrorString() << std::endl;
        return std::nullopt;
    }

    ModelData::ModelT data;

    // process materials
    for (uint32_t i = 0; i < scene->mNumMaterials; i++)
    {
        data.materials.push_back(ProcessMaterial(scene->mMaterials[i]));
    }

    // process meshes
    for (uint32_t i = 0; i < scene->mNumMeshes; i++)
    {
        data.meshes.push_back(ProcessMesh(scene->mMeshes[i]));
    }

    // recursively process tree structure
    data.tree = ProcessTree(scene->mRootNode);

    return data;
}
