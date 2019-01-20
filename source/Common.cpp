#include "Common.h"
#include <SDL.h>
#include <cassert>
#include <cstdio>
#include <chrono>
#include <inttypes.h>
#include <cmath>

extern SDL_Window* g_window;

namespace Common
{
    std::vector<uint8_t> ReadFile(const char * name)
    {
        std::vector<uint8_t> result;

        SDL_RWops * file = SDL_RWFromFile(name, "rb");

        if (!file)
        {
            printf("Error opening file %s\n", name);
            return result;
        }

        Sint64 length = SDL_RWseek(file, 0, RW_SEEK_END);
        SDL_RWseek(file, 0, RW_SEEK_SET);

        printf("Reading file %s length %" PRId64 "\n", name, length);

        result.resize((size_t)length);
        auto bytesRead = SDL_RWread(file, result.data(), 1, result.size());
        assert(bytesRead == length);

        SDL_RWclose(file);

        return result;
    }

    std::tuple<int32_t, int32_t> GetWindowSize()
    {
        int32_t width, height;

        SDL_GetWindowSize(g_window, &width, &height);

        return { width, height };
    }

    double GetCurrentTimeInSeconds()
    {
        double currentTime = (double)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        return (double)currentTime / 1000.0f;
    }

    bool IsNear(float a, float b, float sigma)
    {
        return std::fabs(a - b) < sigma;
    }

    std::vector<uint8_t> ConvertBGRToRGB(const uint8_t * data, uint32_t count)
    {
        std::vector<uint8_t> result(count * 3);

        for (uint32_t i = 0; i < count; ++i)
        {
            result[i * 3 + 0] = data[i * 3 + 2];
            result[i * 3 + 1] = data[i * 3 + 1];
            result[i * 3 + 2] = data[i * 3 + 0];
        }

        return result;
    }

    std::vector<uint8_t> ConvertARGBToRGBA(const uint8_t * data, uint32_t count)
    {
        std::vector<uint8_t> result(count * 4);
        memcpy(result.data(), data, count * 4);

        const uint8_t * current = data;

        for (uint32_t i = 0; i < count; ++i)
        {
            result[i * 4] = data[i * 4 + 3];
        }

        return result;
    }

    void ComputeTangentsAndBitangents(const std::vector<glm::vec3> & vertices, const std::vector<glm::vec2> & uvs,
        std::vector<glm::vec3> & tangents, std::vector<glm::vec3> & bitangents)
    {
        for (size_t i = 0; i < vertices.size(); i += 3)
        {
            // Shortcuts for vertices
            const glm::vec3 & v0 = vertices[i + 0];
            const glm::vec3 & v1 = vertices[i + 1];
            const glm::vec3 & v2 = vertices[i + 2];

            // Shortcuts for UVs
            const glm::vec2 & uv0 = uvs[i + 0];
            const glm::vec2 & uv1 = uvs[i + 1];
            const glm::vec2 & uv2 = uvs[i + 2];

            // Edges of the triangle : position delta
            glm::vec3 deltaPos1 = v1 - v0;
            glm::vec3 deltaPos2 = v2 - v0;

            // UV delta
            glm::vec2 deltaUV1 = uv1 - uv0;
            glm::vec2 deltaUV2 = uv2 - uv0;

            float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
            glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y)*r;
            glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x)*r;

            //tangent.x = r * (deltaUV2.y * deltaPos1.x - deltaUV1.y * deltaPos2.x);
            //tangent.y = r * (deltaUV2.y * deltaPos1.y - deltaUV1.y * deltaPos2.y);
            //tangent.z = r * (deltaUV2.y * deltaPos1.z - deltaUV1.y * deltaPos2.z);
            tangent = glm::normalize(tangent);

            //bitangent.x = r * (-deltaUV2.x * deltaPos1.x + deltaUV1.x * deltaPos2.x);
            //bitangent.y = r * (-deltaUV2.x * deltaPos1.y + deltaUV1.x * deltaPos2.y);
            //bitangent.z = r * (-deltaUV2.x * deltaPos1.z + deltaUV1.x * deltaPos2.z);
            bitangent = glm::normalize(bitangent);

            // Set the same tangent for all three vertices of the triangle.
            // They will be merged later in vbo indexer.
            tangents.push_back(tangent);
            tangents.push_back(tangent);
            tangents.push_back(tangent);

            // Same thing for bitangents.
            bitangents.push_back(bitangent);
            bitangents.push_back(bitangent);
            bitangents.push_back(bitangent);
        }
    }

}
