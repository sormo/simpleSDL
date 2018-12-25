#pragma once
#include <optional>
#include "OpenGL.h"

std::optional<GLuint> LoadBMP(const char * imagePath);
std::optional<GLuint> LoadDDS(const char * imagePath);
