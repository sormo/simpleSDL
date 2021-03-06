#pragma once
#include <vector>
#include <string>
#include "glm/glm.hpp"

#if defined(ANDROID)
// Just rerouting printf to logcat on android (not a complete logging solution).
#include <android/log.h>
#define printf(...) __android_log_print(ANDROID_LOG_INFO, "SIMPLE_EXAMPLE", __VA_ARGS__)
#else
#include <cstdio>
#endif

class Camera;

#define COUNTOF(_ARR) ((uint32_t)(sizeof(_ARR)/sizeof(*_ARR)))

namespace Common
{
    std::vector<uint8_t> ReadFile(const char * name);
    std::string ReadFileToString(const char * name);
    
    bool WriteFile(const char* name, const std::vector<uint8_t>& data);
    bool AppendFile(const char* name, const std::vector<uint8_t>& data);

    std::tuple<int32_t, int32_t> GetWindowSize();
    int32_t GetWindowWidth();
    int32_t GetWindowHeight();

    // seconds since epoch
    double GetCurrentTimeInSeconds();

    // directory if not empty, will be delimited with '/'
    std::string GetDirectoryFromFilePath(const std::string & filePath);

    // reading integral types from buffer
    template<class T>
    T BufferRead(const std::vector<uint8_t> & data, uint32_t offset)
    {
        T value = 0;
        memcpy(&value, &(data[offset]), sizeof(T));
        return value;
    }

    std::vector<uint8_t> ConvertBGRToRGB(const uint8_t * data, uint32_t count);
    std::vector<uint8_t> ConvertARGBToRGBA(const uint8_t * data, uint32_t count);

    namespace Frame
    {
        void Signal();
        float GetFPS();
    }

    // from depth value [0, 1] make distance [nearPlane, farPlane]
    float LinearizeDepth(float depth, float nearPlane, float farPlane);
    // from distance [nearPlane, farPlane] make depth [0, 1]
    float MakeDepth(float distance, float nearPlane, float farPlane);
    // from screen point ([0,0] in upper left) make point in world space with distance from camera (in [nearPlane, farPlane])
    glm::vec3 GetPointWorldSpace(const glm::vec2 & position, float distance, const Camera & camera);
    // TODO those two doesn't work correcly
    glm::vec3 GetPointWorldSpaceManual(const glm::vec2 & position, float distance, const Camera & camera);
    glm::vec3 GetPointWorldSpaceDirection(const glm::vec2 & position, float distance, const Camera & camera);

    namespace Math
    {
        static const float DEFAULT_SIGMA = 0.01f;

        bool IsPowerOfTwo(int32_t n);
        bool IsNear(float a, float b, float sigma = DEFAULT_SIGMA);

        struct Line
        {
            static Line CreateFromPoints(const glm::vec3 & p1, const glm::vec3 & p2);

            // parameter t (of R)
            // line: x = point.x + vector.x * t
            //       y = point.y + vector.y * t
            //       z = point.z + vector.z * t

            glm::vec3 vector;
            glm::vec3 point;
        };

        struct Plane
        {
            static Plane CreateFromPoints(const glm::vec3 & p1, const glm::vec3 & p2, const glm::vec3 & p3);
            void Translate(const glm::vec3 & vector);
            // rotation will "displace" plane, 
            void Rotate(const glm::vec3 & radians);
            // move plane such point is contained within plane
            void Position(const glm::vec3& point);

            // plane: normal.x * x + normal.y * y + normal.z * z - value = 0;

            glm::vec3 normal;
            float value;
        };

        glm::vec3 GetIntersection(const Plane & plane, const Line & line);

        glm::vec3 GetRotation(float radians, const glm::vec3 & axis);
        glm::vec3 RotateRotation(const glm::vec3& rotation, float radians, const glm::vec3& axis);
        glm::vec3 RotateVector(const glm::vec3& vector, const glm::vec3& rotation);

        float GetAngle(const glm::vec3& p1, const glm::vec3& p2);
        // signed angle according to plane normal
        float GetAngle(const glm::vec3& p1, const glm::vec3& p2, const Plane & plane);

        float GetDistance(const Plane& plane, const glm::vec3& p);

        glm::vec3 GetClosestPointOnLine(const Line& line, const glm::vec3& p);
    }

    // get ray from camera going through position in screen coordinates
    Math::Line GetRay(const glm::vec2 & position, const Camera & camera);

    template<class T>
    std::vector<T> MergeVectors(const std::vector<T> & v1, const std::vector<T> & v2)
    {
        std::vector<T> result;
        result.reserve(v1.size() + v2.size());
        result.insert(result.end(), v1.begin(), v1.end());
        result.insert(result.end(), v2.begin(), v2.end());
        return result;
    }
}
