#pragma once
#include <vector>

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
}
