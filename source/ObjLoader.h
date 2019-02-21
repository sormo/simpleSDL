#pragma once
#include <vector>
#include "glm\glm.hpp"

bool LoadObj(const char * path,
             std::vector<glm::vec3> & vertices,
             std::vector<glm::vec2> & uvs,
             std::vector<glm::vec3> & normals);
