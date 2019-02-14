#pragma once
#include "OpenGL.h"
#include <vector>
#include <optional>
#include <map>
#include <string>

class TextureManager
{
public:
    ~TextureManager();
    // TODO will be textures correctly unloaded at the end od process ???
    static TextureManager & Instance();

    std::optional<GLuint> GetTexture(const char * path);

private:
    std::map<std::string, GLuint> m_textures;
};
