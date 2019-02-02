#pragma once
#include "glm/glm.hpp"
#include <vector>

void ComputeTangentsAndBitangents(const std::vector<glm::vec3> & vertices, const std::vector<glm::vec2> & uvs,
                                  std::vector<glm::vec3> & tangents, std::vector<glm::vec3> & bitangents);

void ComputeTangentsAndBitangents(const glm::vec3 * vertices, const glm::vec2 * uvs, size_t size,
                                  std::vector<glm::vec3> & tangents, std::vector<glm::vec3> & bitangents);
