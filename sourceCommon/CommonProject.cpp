#include "CommonProject.h"

glm::vec3 Convert(const ModelData::Color & color)
{
    return glm::vec3{ color.r(), color.g(), color.b() };
}
