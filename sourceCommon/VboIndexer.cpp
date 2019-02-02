#include "VboIndexer.h"
#include <optional>

static const float DEFAULT_SIGMA = 0.01f;
bool IsNear(float a, float b, float sigma = DEFAULT_SIGMA)
{
    return std::fabs(a - b) < sigma;
}

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
        if (IsNear(vertex.x, vertices[i].x) &&
            IsNear(vertex.y, vertices[i].y) &&
            IsNear(vertex.z, vertices[i].z) &&
            IsNear(uv.x, uvs[i].x) &&
            IsNear(uv.y, uvs[i].y) &&
            IsNear(normal.x, normals[i].x) &&
            IsNear(normal.y, normals[i].y) &&
            IsNear(normal.z, normals[i].z))
        {
            return static_cast<uint16_t>(i);
        }
    }
    // No other vertex could be used instead.
    // Looks like we'll have to add it to the VBO.
    return std::nullopt;
}

IndexingResult VboIndex(const glm::vec3 * vertices,
                        const glm::vec2 * uvs,
                        const glm::vec3 * normals,
                        const glm::vec3 * tangents,
                        const glm::vec3 * bitangents,
                        size_t size)
{
    IndexingResult result;

    // For each input vertex
    for (size_t i = 0; i < size; i++)
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

IndexingResult VboIndex(const std::vector<glm::vec3> & vertices,
                        const std::vector<glm::vec2> & uvs,
                        const std::vector<glm::vec3> & normals,
                        const std::vector<glm::vec3> & tangents,
                        const std::vector<glm::vec3> & bitangents)
{
    return VboIndex(vertices.data(), uvs.data(), normals.data(), tangents.data(), bitangents.data(), vertices.size());
}

std::optional<uint16_t> GetSimilarVertexIndex(const glm::vec3 & vertex, const std::vector<glm::vec3> & vertices)
{
    // Lame linear search
    for (size_t i = 0; i < vertices.size(); i++)
    {
        if (IsNear(vertex.x, vertices[i].x) &&
            IsNear(vertex.y, vertices[i].y) &&
            IsNear(vertex.z, vertices[i].z))
        {
            return static_cast<uint16_t>(i);
        }
    }
    // No other vertex could be used instead.
    // Looks like we'll have to add it to the VBO.
    return std::nullopt;
}

IndexingResult VboIndex(const glm::vec3 * vertices, size_t size)
{
    IndexingResult result;

    // For each input vertex
    for (size_t i = 0; i < size; i++)
    {
        if (auto found = GetSimilarVertexIndex(vertices[i], result.vertices))
        {
            // A similar vertex is already in the VBO, use it instead !
            result.indices.push_back(*found);
        }
        else
        {
            // If not, it needs to be added in the output data.
            result.vertices.push_back(vertices[i]);
            result.indices.push_back((uint16_t)result.vertices.size() - 1);
        }
    }

    return result;
}

IndexingResult VboIndex(const std::vector<glm::vec3> & vertices)
{
    return VboIndex(vertices.data(), vertices.size());
}

IndexingResult VboIndex(const glm::vec3 * vertices,
                        const glm::vec2 * uvs,
                        const glm::vec3 * normals,
                        size_t size)
{
    IndexingResult result;

    // For each input vertex
    for (size_t i = 0; i < size; i++)
    {
        if (auto found = GetSimilarVertexIndex(vertices[i], uvs[i], normals[i], result.vertices, result.uvs, result.normals))
        {
            // A similar vertex is already in the VBO, use it instead !
            result.indices.push_back(*found);
        }
        else
        {
            // If not, it needs to be added in the output data.
            result.vertices.push_back(vertices[i]);
            result.uvs.push_back(uvs[i]);
            result.normals.push_back(normals[i]);
            result.indices.push_back((uint16_t)result.vertices.size() - 1);
        }
    }

    return result;
}

IndexingResult VboIndex(const std::vector<glm::vec3> & vertices,
                        const std::vector<glm::vec2> & uvs,
                        const std::vector<glm::vec3> & normals)
{
    return VboIndex(vertices.data(), uvs.data(), normals.data(), vertices.size());
}
