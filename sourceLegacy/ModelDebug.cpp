#include "ModelDebug.h"

#include "debug_draw.hpp"

ModelDebug::ModelDebug(const ModelData::MeshT & mesh)
{
    size_t verticesSize = mesh.positions.size();
    size_t indicesSize = mesh.indices.size();

    const glm::vec3 * positions = (const glm::vec3 *)mesh.positions.data();
    const glm::vec3 * normals = (const glm::vec3 *)mesh.normals.data();
    const glm::vec2 * texCoords = (const glm::vec2 *)mesh.texCoords.data();
    const glm::vec3 * tangents = (const glm::vec3 *)mesh.tangents.data();
    const glm::vec3 * bitangents = (const glm::vec3 *)mesh.bitangents.data();
    const uint16_t * indices = (const uint16_t *)mesh.indices.data();

    m_positions.assign(positions, positions + verticesSize);
    m_normals.assign(normals, normals + verticesSize);
    m_texCoords.assign(texCoords, texCoords + verticesSize);
    m_tangents.assign(tangents, tangents + verticesSize);
    m_bitangents.assign(bitangents, bitangents + verticesSize);
    m_indices.assign(indices, indices + indicesSize);
}

void ModelDebug::Draw()
{
    //for (size_t i = 0; i < m_positions.size(); ++i)
    //{
    //    dd::tangentBasis((float*)&m_positions[i], (float*)&m_normals[i], (float*)&m_tangents[i], (float*)&m_bitangents[i], 2.5f);
    //    dd::point((float*)&m_positions[i], dd::colors::White, 15.0f);
    //}
    for (size_t i = 0; i < 3; ++i)
    {
        dd::tangentBasis((float*)&m_positions[m_indices[i]], (float*)&m_normals[m_indices[i]], (float*)&m_tangents[m_indices[i]], (float*)&m_bitangents[m_indices[i]], 2.5f);
        dd::point((float*)&m_positions[m_indices[i]], dd::colors::White, 15.0f);
    }
}
