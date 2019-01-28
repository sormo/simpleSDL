#include "ModelLoader.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/DefaultLogger.hpp>

#include <string>
#include <iostream>
#include <vector>

ModelData::TextureType MapAssimpTextureTypeToModel(aiTextureType textureType)
{
    switch (textureType)
    {
    case aiTextureType_DIFFUSE:
        return ModelData::TextureType_Diffuse;
    case aiTextureType_SPECULAR:
        return ModelData::TextureType_Specular;
    case aiTextureType_NORMALS:
        return ModelData::TextureType_Normal;
    case aiTextureType_AMBIENT:
        return ModelData::TextureType_Ambient;
    case aiTextureType_HEIGHT:
        return ModelData::TextureType_Height;
    }
    assert(false);
    return ModelData::TextureType_MAX;
}

// checks all material textures of a given type and loads the textures if they're not loaded yet.
// the required info is returned as a Texture struct.
std::vector<std::unique_ptr<ModelData::TextureT>> ProcessTexture(aiMaterial * material, aiTextureType textureType)
{
    std::vector<std::unique_ptr<ModelData::TextureT>> result;

    for (uint32_t i = 0; i < material->GetTextureCount(textureType); i++)
    {
        aiString texturePath;
        material->GetTexture(textureType, i, &texturePath);

        std::unique_ptr<ModelData::TextureT> texture = std::make_unique<ModelData::TextureT>();

        texture->type = MapAssimpTextureTypeToModel(textureType);
        texture->path = texturePath.C_Str();
        result.push_back(std::move(texture));
    }

    return result;
}

std::unique_ptr<ModelData::MeshT> ProcessMesh(aiMesh * mesh, const aiScene * scene)
{
    // data to fill
    std::unique_ptr<ModelData::MeshT> result = std::make_unique<ModelData::MeshT>();

    // Walk through each of the mesh's vertices
    for (uint32_t i = 0; i < mesh->mNumVertices; i++)
    {
        result->positions.push_back({ mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z });
        result->normals.push_back({ mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z });
        result->texCoords.push_back({ mesh->mTextureCoords[0] ? mesh->mTextureCoords[0][i].x : 0.0f, mesh->mTextureCoords[0] ? mesh->mTextureCoords[0][i].y : 0.0f });
        result->tangents.push_back({ mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z });
        result->bitangents.push_back({ mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z });
    }

    // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
    for (uint32_t i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        // retrieve all indices of the face and store them in the indices vector
        for (uint32_t j = 0; j < face.mNumIndices; j++)
            result->indices.push_back(face.mIndices[j]);
    }

    // process materials
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

    // 1. diffuse maps
    auto diffuse = ProcessTexture(material, aiTextureType_DIFFUSE);
    result->textures.insert(result->textures.end(), std::make_move_iterator(diffuse.begin()), std::make_move_iterator(diffuse.end()));
    // 2. specular maps
    auto specular = ProcessTexture(material, aiTextureType_SPECULAR);
    result->textures.insert(result->textures.end(), std::make_move_iterator(specular.begin()), std::make_move_iterator(specular.end()));
    // 3. normal maps
    auto normal = ProcessTexture(material, aiTextureType_NORMALS);
    result->textures.insert(result->textures.end(), std::make_move_iterator(normal.begin()), std::make_move_iterator(normal.end()));
    // 4. ambient maps
    auto ambient = ProcessTexture(material, aiTextureType_AMBIENT);
    result->textures.insert(result->textures.end(), std::make_move_iterator(ambient.begin()), std::make_move_iterator(ambient.end()));
    // 5. height maps
    auto height = ProcessTexture(material, aiTextureType_HEIGHT);
    result->textures.insert(result->textures.end(), std::make_move_iterator(height.begin()), std::make_move_iterator(height.end()));

    // return a mesh object created from the extracted mesh data
    return result;
}

void ProcessNode(aiNode * node, const aiScene * scene, ModelData::ModelT & model)
{
    // process each mesh located at the current node
    for (uint32_t i = 0; i < node->mNumMeshes; i++)
    {
        // the node object only contains indices to index the actual objects in the scene. 
        // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
        aiMesh * mesh = scene->mMeshes[node->mMeshes[i]];
        model.meshes.push_back(ProcessMesh(mesh, scene));
    }
    // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        ProcessNode(node->mChildren[i], scene, model);
    }

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

    const aiScene * scene = importer.ReadFile(path, aiProcessPreset_TargetRealtime_MaxQuality);
    
    DestroyLogger();
    
    // check for errors
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "Error assimp: " << importer.GetErrorString() << std::endl;
        return std::nullopt;
    }
    ModelData::ModelT data;

    // process ASSIMP's root node recursively
    ProcessNode(scene->mRootNode, scene, data);

    return data;
}
