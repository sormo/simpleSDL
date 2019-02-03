#include "Texture.h"
#include "Common.h"
#include <vector>
#include <string>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

namespace Texture
{
    std::optional<GLuint> LoadBMP(const char * imagePath)
    {
        printf("Reading image %s\n", imagePath);

        std::vector<uint8_t> data = Common::ReadFile(imagePath);
        if (data.empty())
        {
            printf("Error reading file %s.\n", imagePath);
            return std::nullopt;
        }

        if (data.size() < 54)
        {
            printf("Error, size of BMP file is only %u.\n", data.size());
            return std::nullopt;
        }

        // A BMP files always begins with "BM"
        if (data[0] != 'B' || data[1] != 'M') {
            printf("Incorrect first two bytes in BMP file.\n");
            return std::nullopt;
        }

        // Read the information about the image
        uint32_t fileSize = Common::BufferRead<uint32_t>(data, 0x02);
        uint32_t dataPosition = Common::BufferRead<uint32_t>(data, 0x0a);
        uint32_t dibHeaderSize = Common::BufferRead<uint32_t>(data, 0x0e);

        static const uint32_t BITMAPV4HEADER = 0x6c;
        static const uint32_t BITMAPV5HEADER = 0x7c;
        static const uint32_t BITMAPINFOHEADER = 0x28;

        if (dibHeaderSize != BITMAPV4HEADER && dibHeaderSize != BITMAPV5HEADER && dibHeaderSize != BITMAPINFOHEADER)
        {
            printf("Unsupported DIB header size %d.\n", dibHeaderSize);
            return std::nullopt;
        }

        uint32_t bitmapWidth = Common::BufferRead<uint32_t>(data, 0x12);
        uint32_t bitmapHeight = Common::BufferRead<uint32_t>(data, 0x16);
        uint16_t bitmapBpp = Common::BufferRead<uint16_t>(data, 0x1c);
        uint32_t bitmapSize = Common::BufferRead<uint32_t>(data, 0x22);

        if (bitmapBpp != 24 && bitmapBpp != 32)
        {
            printf("Unsupported BMP bpp %d.\n", bitmapBpp);
            return std::nullopt;
        }

        // Some BMP files are misformatted, guess missing information
        if (bitmapSize == 0)
            bitmapSize = bitmapWidth * bitmapHeight * (bitmapBpp / 4); // this may not be correct

        // Create one OpenGL texture
        GLuint textureID;
        glGenTextures(1, &textureID);

        //glActiveTexture(GL_TEXTURE0);

        // "Bind" the newly created texture : all future texture functions will modify this texture
        glBindTexture(GL_TEXTURE_2D, textureID);

        // TODO: row of bitmap file must be multiple of 4, if it's not, there can be padding

        std::vector<uint8_t> bitmapData;
        GLenum format;
        if (bitmapBpp == 24)
        {
            format = GL_RGB;
            bitmapData = Common::ConvertBGRToRGB(data.data() + dataPosition, bitmapWidth * bitmapHeight);
        }
        else if (bitmapBpp == 32)
        {
            format = GL_RGBA;
            bitmapData = Common::ConvertARGBToRGBA(data.data() + dataPosition, bitmapWidth * bitmapHeight);
        }

        glTexImage2D(GL_TEXTURE_2D, 0, format, bitmapWidth, bitmapHeight, 0, format, GL_UNSIGNED_BYTE, bitmapData.data());

        // Poor filtering, or ...
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 

        // ... nice trilinear filtering.
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glGenerateMipmap(GL_TEXTURE_2D);

        // Return the ID of the texture we just created
        return textureID;
    }

#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII

    bool ValidateCompressedTextureFormat(uint32_t format)
    {
        printf("Compressed texture format being used: %d\n", format);

        // check for texture formats:
        GLint numFormats = 0;
        glGetIntegerv(GL_NUM_COMPRESSED_TEXTURE_FORMATS, &numFormats);

        if (numFormats == 0)
        {
            printf("No compressed texture formats are supported.");
            return false;
        }

        std::vector<GLint> formats(numFormats);
        glGetIntegerv(GL_COMPRESSED_TEXTURE_FORMATS, formats.data());
        for (GLint f : formats)
        {
            if (f == format)
                return true;
        }

        printf("Compressed texture format not supported.\n");

        std::string formatsStr;
        for (GLint f : formats)
        {
            formatsStr.append(std::to_string(f) + " ");
        }

        printf("Only following format are supported: %s", formatsStr.c_str());

        return false;
    }

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

        if (!ValidateCompressedTextureFormat(format))
            return std::nullopt;

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

    std::optional<GLuint> Load(const char * imagePath)
    {
        printf("Reading image %s\n", imagePath);

        std::vector<uint8_t> data = Common::ReadFile(imagePath);

        if (data.empty())
        {
            printf("Error reading file %s", imagePath);
            return std::nullopt;
        }

        int32_t width, height, numberOfColorChannels;

        stbi_set_flip_vertically_on_load(true);

        uint8_t * imageData = stbi_load_from_memory(data.data(), data.size(), &width, &height, &numberOfColorChannels, 0);
        if (!imageData)
        {
            printf("Error loading image using STB.");
            return std::nullopt;
        }

        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        // set the texture wrapping/filtering options (on the currently bound texture object)
        // warning: emscripten accepts only GL_CLAMP_TO_EDGE for non-power of two textures
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        GLint format;
        switch (numberOfColorChannels)
        {
        case 3:
            format = GL_RGB;
            break;
        case 4:
            format = GL_RGBA;
            break;
        }

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, imageData);
        //glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(imageData);

        return texture;
    }
}
