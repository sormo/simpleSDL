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

    std::string ReadFileToString(const char * name)
    {
        auto data = ReadFile(name);
        data.push_back('\0');

        return (const char *)data.data();
    }

    std::tuple<int32_t, int32_t> GetWindowSize()
    {
        int32_t width, height;

        SDL_GetWindowSize(g_window, &width, &height);

        return { width, height };
    }

    int32_t GetWindowWidth()
    {
        int32_t width;

        SDL_GetWindowSize(g_window, &width, nullptr);

        return width;
    }

    int32_t GetWindowHeight()
    {
        int32_t height;

        SDL_GetWindowSize(g_window, nullptr, &height);

        return height;
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

    std::string GetDirectoryFromFilePath(const std::string & filePath)
    {
        auto pos = filePath.find_last_of("\\/");
        if (pos == std::string::npos)
            return "";
        return filePath.substr(0, pos) + "/";
    }

    bool IsPowerOfTwo(int32_t n)
    {
        n = std::abs(n);

        if (n == 0 || n == 1)
            return false;

        while (n != 1)
        {
            if (n % 2)
                return false;
            n /= 2;
        }

        return true;
    }
}
