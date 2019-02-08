#pragma once
#include "glm/glm.hpp"
#include "model_generated.h"
#include <vector>

class ModelDebug
{
public:
    ModelDebug(const ModelData::MeshT & mesh);

    void Draw();
private:
    std::vector<glm::vec3> m_positions;
    std::vector<glm::vec3> m_normals;
    std::vector<glm::vec2> m_texCoords;
    std::vector<glm::vec3> m_tangents;
    std::vector<glm::vec3> m_bitangents;
    std::vector<uint16_t> m_indices;

};
