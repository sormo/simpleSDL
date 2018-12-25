#include "Texture.h"
#include "Common.h"
#include <vector>

std::optional<GLuint> LoadBMP(const char * imagePath)
{
    printf("Reading image %s\n", imagePath);

    std::vector<uint8_t> data = Common::ReadFile(imagePath);
    if (data.empty())
    {
        printf("Error reading file %s", imagePath);
        return std::nullopt;
    }

    if (data.size() < 54)
    {
        printf("Error, size of BMP file is only %u", data.size());
        return std::nullopt;
    }

    // A BMP files always begins with "BM"
    if (data[0] != 'B' || data[1] != 'M') {
        printf("Not a correct BMP file");
        return std::nullopt;
    }

    // Make sure this is a 24bpp file
    if (*(int32_t*)&(data[0x1E]) != 0)
    {
        printf("Not a correct BMP file\n");
        return std::nullopt;
    }
    if (*(int32_t*)&(data[0x1C]) != 24)
    {
        printf("Not a correct BMP file\n");
        return std::nullopt;
    }

    // Read the information about the image
    int32_t dataPos = *(int32_t*)&(data[0x0A]);
    int32_t imageSize = *(int32_t*)&(data[0x22]);
    int32_t width = *(int32_t*)&(data[0x12]);
    int32_t height = *(int32_t*)&(data[0x16]);

    // Some BMP files are misformatted, guess missing information
    if (imageSize == 0)
        imageSize = width * height * 3; // 3 : one byte for each Red, Green and Blue component
    if (dataPos == 0)
        dataPos = 54; // The BMP header is done that way

    // Create one OpenGL texture
    GLuint textureID;
    glGenTextures(1, &textureID);

    // "Bind" the newly created texture : all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, textureID);

    GLvoid * BMPData = (GLvoid*)(data.data() + dataPos);
    // Give the image to OpenGL
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, BMPData);

    // Poor filtering, or ...
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 

    // ... nice trilinear filtering.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);

    // Return the ID of the texture we just created
    return textureID;
}

#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII

std::optional<GLuint> LoadDDS(const char * imagePath)
{
    printf("Reading image %s\n", imagePath);

    std::vector<uint8_t> data = Common::ReadFile(imagePath);
    
    if (data.empty())
    {
        printf("Error reading file %s", imagePath);
        return std::nullopt;
    }

    if (data.size() < 128)
    {
        printf("Error, size of DDS file is only %u", data.size());
        return std::nullopt;
    }

    if (data[0] != 'D' || data[1] != 'D' || data[2] != 'S' || data[3] != ' ')
    {
        printf("Error, invalid DDS header.");
        return std::nullopt;
    }

    uint8_t * header = &data[4];

    uint32_t height = *(uint32_t*)&(header[8]);
    uint32_t width = *(uint32_t*)&(header[12]);
    uint32_t linearSize = *(uint32_t*)&(header[16]);
    uint32_t mipMapCount = *(uint32_t*)&(header[24]);
    uint32_t fourCC = *(uint32_t*)&(header[80]);
    uint32_t bufsize = mipMapCount > 1 ? linearSize * 2 : linearSize;
    /* how big is it going to be including all mipmaps? */
    
    uint8_t * buffer = header + 124;

    unsigned int components = (fourCC == FOURCC_DXT1) ? 3 : 4;
    unsigned int format;
    switch (fourCC)
    {
    case FOURCC_DXT1:
        format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
        break;
    case FOURCC_DXT3:
        format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
        break;
    case FOURCC_DXT5:
        format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
        break;
    default:
        return std::nullopt;
    }

    // Create one OpenGL texture
    GLuint textureID;
    glGenTextures(1, &textureID);

    // "Bind" the newly created texture : all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, textureID);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    uint32_t blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
    uint32_t offset = 0;

    /* load the mipmaps */
    for (uint32_t level = 0; level < mipMapCount && (width || height); ++level)
    {
        uint32_t size = ((width + 3) / 4)*((height + 3) / 4)*blockSize;
        glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height,
            0, size, buffer + offset);

        offset += size;
        width /= 2;
        height /= 2;

        // Deal with Non-Power-Of-Two textures. This code is not included in the webpage to reduce clutter.
        if (width < 1)
            width = 1;
        if (height < 1)
            height = 1;
    }

    return textureID;
}
