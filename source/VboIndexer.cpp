#include "VboIndexer.h"
#include "Common.h"
#include <optional>

std::optional<uint16_t> GetSimilarVertexIndex(
    const glm::vec3 & vertex,
    const glm::vec2 & uv,
    const glm::vec3 & normal,
    const std::vector<glm::vec3> & vertices,
    const std::vector<glm::vec2> & uvs,
    const std::vector<glm::vec3> & normals)
{
    // Lame linear search
    for (size_t i = 0; i < vertices.size(); i++)
    {
        if (Common::IsNear(vertex.x, vertices[i].x) &&
            Common::IsNear(vertex.y, vertices[i].y) &&
            Common::IsNear(vertex.z, vertices[i].z) &&
            Common::IsNear(uv.x, uvs[i].x) &&
            Common::IsNear(uv.y, uvs[i].y) &&
            Common::IsNear(normal.x, normals[i].x) &&
            Common::IsNear(normal.y, normals[i].y) &&
            Common::IsNear(normal.z, normals[i].z))
        {
            return static_cast<uint16_t>(i);
        }
    }
    // No other vertex could be used instead.
    // Looks like we'll have to add it to the VBO.
    return std::nullopt;
}

IndexingResult VboIndex(const std::vector<glm::vec3> & vertices,
                        const std::vector<glm::vec2> & uvs,
                        const std::vector<glm::vec3> & normals,
                        const std::vector<glm::vec3> & tangents,
                        const std::vector<glm::vec3> & bitangents)
{
    IndexingResult result;

    // For each input vertex
    for (size_t i = 0; i < vertices.size(); i++)
    {
        if (auto found = GetSimilarVertexIndex(vertices[i], uvs[i], normals[i], result.vertices, result.uvs, result.normals))
        {
            // A similar vertex is already in the VBO, use it instead !
            result.indices.push_back(*found);

            // Average the tangents and the bitangents
            result.tangents[*found] += tangents[i];
            result.bitangents[*found] += bitangents[i];
        }
        else
        {
            // If not, it needs to be added in the output data.
            result.vertices.push_back(vertices[i]);
            result.uvs.push_back(uvs[i]);
            result.normals.push_back(normals[i]);
            result.tangents.push_back(tangents[i]);
            result.bitangents.push_back(bitangents[i]);
            result.indices.push_back((uint16_t)result.vertices.size() - 1);
        }
    }

    return result;
}
