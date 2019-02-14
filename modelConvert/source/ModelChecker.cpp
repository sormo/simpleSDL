#include "ModelChecker.h"
#include <iostream>
#include "TangentSpace.h"
#include "VboIndexer.h"

void ValidateWindingOrders(glm::vec3 * positions, glm::vec3 * normals, std::vector<uint16_t> & indices)
{
    for (size_t i = 0; i < indices.size(); i += 3)
    {
        uint16_t a = indices[i];
        uint16_t b = indices[i + 1];
        uint16_t c = indices[i + 2];

        glm::vec3 N = glm::cross(positions[b] - positions[a], positions[c] - positions[a]);
        float w = glm::dot(N, positions[a] - normals[a]);

        // want CCW for front face
        if (w < 0.0f)
        {
            std::iter_swap(std::begin(indices) + i + 1, std::begin(indices) + i + 2);
        }
    }
}

void UnIndexMesh(ModelData::MeshT & mesh)
{
    size_t size = mesh.indices.size();

    if (size == 0)
        return;

    std::vector<ModelData::Vec3> positions(size);
    std::vector<ModelData::Vec3> normals(size);
    std::vector<ModelData::Vec2> texCoords(size);
    std::vector<ModelData::Vec3> tangents(size);
    std::vector<ModelData::Vec3> bitangents(size);

    for (size_t i = 0; i < size; ++i)
    {
        positions[i] = mesh.positions[mesh.indices[i]];
        normals[i] = mesh.normals[mesh.indices[i]];

        if (!mesh.texCoords.empty())
            texCoords[i] = mesh.texCoords[mesh.indices[i]];

        if (!mesh.tangents.empty())
        {
            tangents[i] = mesh.tangents[mesh.indices[i]];
            bitangents[i] = mesh.bitangents[mesh.indices[i]];
        }
    }

    mesh.positions = positions;
    mesh.normals = normals;

    if (!mesh.texCoords.empty())
        mesh.texCoords = texCoords;

    if (!mesh.tangents.empty())
    {
        mesh.tangents = tangents;
        mesh.bitangents = bitangents;
    }
    mesh.indices.clear();
}

void ComputeTangentSpace(ModelData::MeshT & mesh)
{
    size_t dataSize = mesh.positions.size();

    std::vector<glm::vec3> tangents, bitangents;
    ComputeTangentsAndBitangents((const glm::vec3*)mesh.positions.data(), 
        (const glm::vec2*)mesh.texCoords.data(), dataSize, tangents, bitangents);

    mesh.tangents.resize(dataSize);
    memcpy(mesh.tangents.data(), tangents.data(), sizeof(glm::vec3)*dataSize);
    mesh.bitangents.resize(dataSize);
    memcpy(mesh.bitangents.data(), bitangents.data(), sizeof(glm::vec3)*dataSize);
}

void ComputeIndices(ModelData::MeshT & mesh)
{
    // just index it
    auto result = VboIndex((const glm::vec3*)mesh.positions.data(),
        (const glm::vec2*)mesh.texCoords.data(),
        (const glm::vec3*)mesh.normals.data(),
        (const glm::vec3*)mesh.tangents.data(),
        (const glm::vec3*)mesh.bitangents.data(), mesh.positions.size());

    size_t newDataSize = result.vertices.size();
    size_t newIndicesSize = result.indices.size();

    mesh.indices.resize(newIndicesSize);
    memcpy(mesh.indices.data(), result.indices.data(), sizeof(uint16_t)*newIndicesSize);

    mesh.positions.resize(newDataSize);
    memcpy(mesh.positions.data(), result.vertices.data(), sizeof(glm::vec3)*newDataSize);

    mesh.texCoords.resize(newDataSize);
    memcpy(mesh.texCoords.data(), result.uvs.data(), sizeof(glm::vec2)*newDataSize);

    mesh.normals.resize(newDataSize);
    memcpy(mesh.normals.data(), result.normals.data(), sizeof(glm::vec3)*newDataSize);

    mesh.tangents.resize(newDataSize);
    memcpy(mesh.tangents.data(), result.tangents.data(), sizeof(glm::vec3)*newDataSize);

    mesh.bitangents.resize(newDataSize);
    memcpy(mesh.bitangents.data(), result.bitangents.data(), sizeof(glm::vec3)*newDataSize);
}

bool CheckModel(ModelData::ModelT & model)
{
    auto it = model.meshes.begin();

    while (it != model.meshes.end())
    {
        if (it->get()->positions.empty())
        {
            std::cout << "No positions. Invalid mesh. Remove ..." << std::endl;
            it = model.meshes.erase(it);
            continue;
        }

        if (it->get()->tangents.empty())
        {
            if (!it->get()->texCoords.empty())
            {
                std::cout << "No tangent space. Recompute ..." << std::endl;
                UnIndexMesh(*it->get());
                ComputeTangentSpace(*it->get());
            }
        }

        if (it->get()->indices.empty())
        {
            ComputeIndices(*it->get());
        }

        ValidateWindingOrders((glm::vec3*)it->get()->positions.data(), (glm::vec3*)it->get()->normals.data(), it->get()->indices);

        it++;
    }

    return true;
}
