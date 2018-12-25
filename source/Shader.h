#pragma once
#include "OpenGL.h"
#include <optional>

std::optional<GLuint> CreateProgram(const char * vertexFile, const char * fragmentFile);
