#include "TextureManager.h"
#include "utils/Texture.h"

TextureManager & TextureManager::Instance()
{
    static TextureManager instance;

    return instance;
}

TextureManager::~TextureManager()
{
    for (auto[_, texture] : m_textures)
        glDeleteTextures(1, &texture);
}

std::optional<GLuint> TextureManager::GetTexture(const char * path)
{
    auto it = m_textures.find(path);
    if (it != std::end(m_textures))
        return it->second;

    auto newTexture = Texture::Load(path);
    if (!newTexture)
    {
        printf("Error loading texture %s.\n", path);
        return std::nullopt;
    }
    
    m_textures[path] = *newTexture;

    return newTexture;
}
