#pragma once
#include <optional>
#include "OpenGL.h"

namespace Texture
{
    std::optional<GLuint> LoadBMP(const char * imagePath);
    std::optional<GLuint> LoadDDS(const char * imagePath);

    // load image using STB
    std::optional<GLuint> Load(const char * imagePath);
}
