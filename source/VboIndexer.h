#pragma once
#include <vector>
#include "glm/glm.hpp"

struct IndexingResult
{
    std::vector<uint16_t> indices;
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
};

IndexingResult VboIndex(const std::vector<glm::vec3> & vertices, const std::vector<glm::vec2> & uvs, const std::vector<glm::vec3> & normals);
