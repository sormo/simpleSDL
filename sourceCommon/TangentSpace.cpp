#include "TangentSpace.h"

void ComputeTangentsAndBitangents(const std::vector<glm::vec3> & vertices, const std::vector<glm::vec2> & uvs,
    std::vector<glm::vec3> & tangents, std::vector<glm::vec3> & bitangents)
{
    ComputeTangentsAndBitangents(vertices.data(), uvs.data(), vertices.size(), tangents, bitangents);
}

void ComputeTangentsAndBitangents(const glm::vec3 * vertices, const glm::vec2 * uvs, size_t size,
    std::vector<glm::vec3> & tangents, std::vector<glm::vec3> & bitangents)
{
    for (size_t i = 0; i < size; i += 3)
    {
        // Shortcuts for vertices
        const glm::vec3 & v0 = vertices[i + 0];
        const glm::vec3 & v1 = vertices[i + 1];
        const glm::vec3 & v2 = vertices[i + 2];

        // Shortcuts for UVs
        const glm::vec2 & uv0 = uvs[i + 0];
        const glm::vec2 & uv1 = uvs[i + 1];
        const glm::vec2 & uv2 = uvs[i + 2];

        // Edges of the triangle : position delta
        glm::vec3 deltaPos1 = v1 - v0;
        glm::vec3 deltaPos2 = v2 - v0;

        // UV delta
        glm::vec2 deltaUV1 = uv1 - uv0;
        glm::vec2 deltaUV2 = uv2 - uv0;

        float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
        glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y)*r;
        glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x)*r;

        //tangent.x = r * (deltaUV2.y * deltaPos1.x - deltaUV1.y * deltaPos2.x);
        //tangent.y = r * (deltaUV2.y * deltaPos1.y - deltaUV1.y * deltaPos2.y);
        //tangent.z = r * (deltaUV2.y * deltaPos1.z - deltaUV1.y * deltaPos2.z);
        tangent = glm::normalize(tangent);

        //bitangent.x = r * (-deltaUV2.x * deltaPos1.x + deltaUV1.x * deltaPos2.x);
        //bitangent.y = r * (-deltaUV2.x * deltaPos1.y + deltaUV1.x * deltaPos2.y);
        //bitangent.z = r * (-deltaUV2.x * deltaPos1.z + deltaUV1.x * deltaPos2.z);
        bitangent = glm::normalize(bitangent);

        // Set the same tangent for all three vertices of the triangle.
        // They will be merged later in vbo indexer.
        tangents.push_back(tangent);
        tangents.push_back(tangent);
        tangents.push_back(tangent);

        // Same thing for bitangents.
        bitangents.push_back(bitangent);
        bitangents.push_back(bitangent);
        bitangents.push_back(bitangent);
    }
}
