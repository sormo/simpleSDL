#include "Common.h"
#include <SDL.h>
#include <cassert>
#include <cstdio>
#include <chrono>
#include <inttypes.h>
#include <cmath>
#include "glm/gtc/matrix_transform.hpp"
#include "Camera.h"

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

    namespace Math
    {
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

        bool IsNear(float a, float b, float sigma)
        {
            return std::fabs(a - b) < sigma;
        }

        Line Line::CreateFromPoints(const glm::vec3 & p1, const glm::vec3 & p2)
        {
            Line result;

            result.point = p1;
            result.vector = p2 - p1;

            return result;
        }

        Plane Plane::CreateFromPoints(const glm::vec3 & p1, const glm::vec3 & p2, const glm::vec3 & p3)
        {
            Plane result;

            result.normal = glm::cross(p2 - p1, p3 - p1);
            result.value = -result.normal.x * p1.x - result.normal.y * p1.y - result.normal.z * p1.z;

            return result;
        }

        void Plane::Translate(const glm::vec3 & vector)
        {
            value += normal.x * vector.x + normal.y * vector.y + normal.z * vector.z;
        }

        void Plane::Rotate(const glm::vec3 & radians)
        {
            glm::mat4 model(1.0f);
            model = glm::rotate(model, radians.x, glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::rotate(model, radians.y, glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::rotate(model, radians.z, glm::vec3(0.0f, 0.0f, 1.0f));

            normal = glm::vec3(model * glm::vec4(normal, 0.0f));
        }

        glm::vec3 GetIntersection(const Plane & plane, const Line & line)
        {
            const float & d = plane.value;
            const glm::vec3 & n = plane.normal;
            const glm::vec3 & a = line.point;
            const glm::vec3 & v = line.vector;

            // plug parametric equations of line to euqation of plane and compute t
            float t = (d - n.x * a.x - n.y * a.y - n.z * a.z) / (n.x * v.x + n.y * v.y + n.z * v.z);
            // use t in parametrix equation of line
            glm::vec3 result;

            result = a + v * t;

            return result;
        }
    }

    Math::Line GetRay(const glm::vec2 & position, const Camera & camera)
    {
        Math::Line result;

        result.point = camera.GetPosition();
        result.vector = Common::GetPointWorldSpace(position, 10.0f, camera) - result.point;

        return result;
    }

    namespace Frame
    {
        std::chrono::high_resolution_clock timer;
        std::chrono::time_point<std::chrono::high_resolution_clock> begin;
        std::chrono::time_point<std::chrono::high_resolution_clock> end;

        void Signal()
        {
            begin = end;
            end = timer.now();
        }

        float GetFPS()
        {
            using ms = std::chrono::duration<float, std::milli>;

            static float result = std::chrono::duration_cast<ms>(end - begin).count();

            static int32_t counter = 0;
            if (counter < 100)
            {
                counter++;
                return result;
            }
            counter = 0;

            result = std::chrono::duration_cast<ms>(end - begin).count();

            return result;
        }
    }

    float LinearizeDepth(float depth, float nearPlane, float farPlane)
    {
        depth = 2.0f * depth - 1.0f;
        float linear = 2.0f * nearPlane * farPlane / (farPlane + nearPlane - depth * (farPlane - nearPlane));
        return linear;
    }

    float MakeDepth(float distance, float nearPlane, float farPlane)
    {
        float nonLinear = (farPlane + nearPlane - 2.0f * nearPlane * farPlane / distance) / (farPlane - nearPlane);
        nonLinear = (nonLinear + 1.0f) / 2.0f;
        return nonLinear;
    }

    glm::vec3 GetPointWorldSpace(const glm::vec2 & position, float distance, const Camera & camera)
    {
        float depthValue = MakeDepth(distance, camera.GetPlanes().x, camera.GetPlanes().y);

        glm::vec3 worldPosition = glm::unProject(glm::vec3(position.x, (float)Common::GetWindowHeight() - position.y - 1.0f, depthValue),
            camera.GetViewMatrix(), camera.GetProjectionMatrix(),
            glm::vec4(0.0f, 0.0f, (float)Common::GetWindowWidth(), (float)Common::GetWindowHeight()));

        return worldPosition;
    }

    glm::vec3 GetPointWorldSpaceManual(const glm::vec2 & position, float distance, const Camera & camera)
    {
        glm::vec4 ndc;

        ndc.x = position.x / (float)Common::GetWindowWidth();
        ndc.y = 1.0f - position.y / (float)Common::GetWindowHeight();
        ndc.z = MakeDepth(distance, camera.GetPlanes().x, camera.GetPlanes().y);
        ndc.w = 1.0f;

        ndc = ndc * 2.0f - 1.0f;

        glm::mat4 tranform = camera.GetProjectionMatrix() * camera.GetViewMatrix();
        tranform = glm::inverse(tranform);

        glm::vec4 result = tranform * ndc;

        // perspective division
        result *= (1.0f / result.w);

        return result;
    }

    glm::vec3 GetPointWorldSpaceDirection(const glm::vec2 & position, float distance, const Camera & camera)
    {
        // 0.5 offset is to have ray go through the center of pixel
        glm::vec2 pixelNdc;
        pixelNdc.x = (position.x + 0.5f) / (float)Common::GetWindowWidth();
        pixelNdc.y = (position.y + 0.5f) / (float)Common::GetWindowHeight();

        glm::vec2 pixelScreen;
        pixelScreen.x = 2.0f * pixelNdc.x - 1.0f;
        pixelScreen.y = 1.0f - 2.0f * pixelNdc.y;

        // this code assumes that width > height
        // in other case height should be divided by width and aspectRatio should be multiplied by y coordinate
        float aspectRatio = (float)Common::GetWindowWidth() / (float)Common::GetWindowHeight();
        float fovPart = glm::tan(camera.GetFoV() / 2.0f);

        glm::vec2 pixelCamera;
        pixelCamera.x = pixelScreen.x * aspectRatio * fovPart;
        pixelCamera.y = pixelScreen.y * fovPart;

        // 
        glm::vec3 cameraSpace(pixelCamera.x, pixelCamera.y, -1.0f);

        // origin is in [0,0,0] so no need to substract from it
        glm::vec4 directionCameraSpace(cameraSpace, 0.0f);
        directionCameraSpace = glm::normalize(directionCameraSpace);

        glm::mat4 transform = glm::inverse(camera.GetViewMatrix());

        glm::vec3 directionWorldSpace = transform * directionCameraSpace;

        return directionWorldSpace * distance;
    }
}
