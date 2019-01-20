#pragma once
#include <vector>
#include "glm/glm.hpp"

#if defined(ANDROID)
// Just rerouting printf to logcat on android (not a complete logging solution).
#include <android/log.h>
#define printf(...) __android_log_print(ANDROID_LOG_INFO, "SIMPLE_EXAMPLE", __VA_ARGS__)
#else
#include <cstdio>
#endif

namespace Common
{
    std::vector<uint8_t> ReadFile(const char * name);
    std::tuple<int32_t, int32_t> GetWindowSize();
    // seconds since epoch
    double GetCurrentTimeInSeconds();

    // reading integral types from buffer
    template<class T>
    T BufferRead(const std::vector<uint8_t> & data, uint32_t offset)
    {
        T value = 0;
        memcpy(&value, &(data[offset]), sizeof(T));
        return value;
    }

    static const float DEFAULT_SIGMA = 0.01f;
    bool IsNear(float a, float b, float sigma = DEFAULT_SIGMA);

    std::vector<uint8_t> ConvertBGRToRGB(const uint8_t * data, uint32_t count);
    std::vector<uint8_t> ConvertARGBToRGBA(const uint8_t * data, uint32_t count);

    void ComputeTangentsAndBitangents(const std::vector<glm::vec3> & vertices, const std::vector<glm::vec2> & uvs,
                                      std::vector<glm::vec3> & tangents, std::vector<glm::vec3> & bitangents);
}
