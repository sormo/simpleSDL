#pragma once
#include <optional>
#include <vector>
#include "OpenGL.h"

namespace Texture
{
    std::optional<GLuint> LoadBMP(const char * imagePath);
    std::optional<GLuint> LoadDDS(const char * imagePath);

    // load image using STB
    std::optional<GLuint> Load(const char * imagePath);

    // load cubemap
    // textures in following order: right, left, top, bottom, back, front
    std::optional<GLuint> LoadCubemap(const std::vector<std::string> & paths);
}
