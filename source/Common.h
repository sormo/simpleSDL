#pragma once
#include <vector>

#if defined(ANDROID)
// Just rerouting printf to logcat on android (not a complete logging solution).
#include <android/log.h>
#define printf(...) __android_log_print(ANDROID_LOG_INFO, "SIMPLE_EXAMPLE", __VA_ARGS__)
#endif

namespace Common
{
    std::vector<uint8_t> ReadFile(const char * name);
}
