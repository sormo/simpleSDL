#include "Camera.h"
#include "Common.h"
#include <cmath>
#include "glm/gtc/matrix_transform.hpp"
#include <SDL.h>

#define MOUSE_SPEED 0.005f
#define SPEED 3.0f // 3 units / second

void CameraKeyboard::Init()
{
    RecomputeMatrices();
    Wheel(0.0f);
}

const glm::vec3 & CameraKeyboard::GetPosition() const
{
    return m_position;
}

void CameraKeyboard::RecomputeMatrices()
{
    // glfwGetTime is called only once, the first time this function is called
    static double lastTime = Common::GetCurrentTimeInSeconds();

    // Compute time difference between current and last frame
    double currentTime = Common::GetCurrentTimeInSeconds();
    float deltaTime = float(currentTime - lastTime);

    // Get mouse position
    int32_t mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    auto[width, height] = Common::GetWindowSize();

    // Reset mouse position for next frame
    SDL_WarpMouseInWindow(nullptr, width / 2, height / 2);

    // Compute new orientation
    m_horizontalAngle += MOUSE_SPEED * float(width / 2 - mouseX);
    m_verticalAngle += MOUSE_SPEED * float(height / 2 - mouseY);

    // Direction : Spherical coordinates to Cartesian coordinates conversion
    glm::vec3 direction(
        std::cos(m_verticalAngle) * std::sin(m_horizontalAngle),
        std::sin(m_verticalAngle),
        std::cos(m_verticalAngle) * std::cos(m_horizontalAngle)
    );

    // Right vector
    glm::vec3 right = glm::vec3(
        sin(m_horizontalAngle - 3.14f / 2.0f),
        0,
        cos(m_horizontalAngle - 3.14f / 2.0f)
    );

    // Up vector
    glm::vec3 up = glm::cross(right, direction);

    const Uint8 * keyboardState = SDL_GetKeyboardState(NULL);

    // Move forward
    if (keyboardState[SDL_SCANCODE_UP]) {
        m_position += direction * deltaTime * SPEED;
    }
    // Move backward
    if (keyboardState[SDL_SCANCODE_DOWN]) {
        m_position -= direction * deltaTime * SPEED;
    }
    // Strafe right
    if (keyboardState[SDL_SCANCODE_RIGHT]) {
        m_position += right * deltaTime * SPEED;
    }
    // Strafe left
    if (keyboardState[SDL_SCANCODE_LEFT]) {
        m_position -= right * deltaTime * SPEED;
    }

    // Camera matrix
    m_viewMatrix = glm::lookAt(
        m_position,           // Camera is here
        m_position + direction, // and looks here : at the same position, plus "direction"
        up                  // Head is up (set to 0,-1,0 to look upside-down)
    );

    // For the next frame, the "last time" will be "now"
    lastTime = currentTime;
}

bool CameraKeyboard::Wheel(float value)
{
    auto[width, height] = Common::GetWindowSize();

    m_FoV += 0.1f*value;
    // Projection matrix : 45° Field of View, ratio, display range : 0.1 unit <-> 100 units
    m_projectionMatrix = glm::perspective(m_FoV, (float)width/(float)height, GetPlanes().x, GetPlanes().y);

    return true;
}

const glm::mat4 & CameraKeyboard::GetViewMatrix() const
{
    return m_viewMatrix;
}

const glm::mat4 & CameraKeyboard::GetProjectionMatrix() const
{
    return m_projectionMatrix;
}

void CameraRotate::Init()
{
    m_lookPoint = glm::vec3(0.0f, 0.0f, 0.0f);

    auto[width, height] = Common::GetWindowSize();

    m_projectionMatrix = glm::perspective(m_FoV, (float)width / (float)height, GetPlanes().x, GetPlanes().y);
    RecomputeViewMatrix();
}

const glm::vec3 & CameraRotate::GetPosition() const
{
    return m_position;
}

bool CameraRotate::Press(const glm::vec2 & position, int64_t id)
{
    m_positions[id] = position;

    return true;
}

bool CameraRotate::Release(const glm::vec2 & position, int64_t id)
{
    m_positions.erase(id);

    return true;
}

bool CameraRotate::Move(const glm::vec2 & position, int64_t id)
{
    auto it = m_positions.find(id);
    if (it == std::end(m_positions))
        return false;

    if (m_positions.size() == 1)
    {
        glm::vec2 delta = position - it->second;
        it->second = position;

        auto[width, height] = Common::GetWindowSize();

        m_horizontalAngle -= (delta.x / width) * m_distance;
        m_verticalAngle += (delta.y / height) * m_distance;

        if (m_verticalAngle < -3.14f / 2.0f)
            m_verticalAngle = -3.14f / 2.0f;
        else if (m_verticalAngle > 3.14f / 2.0f)
            m_verticalAngle = 3.14f / 2.0f;
    }
    else if (m_positions.size() == 2)
    {
        // get other position
        auto it1 = std::begin(m_positions);
        auto it2 = std::begin(m_positions); it2++;

        if (it2 == it)
            std::swap(it1, it2);

        float distancePrev = glm::length(it1->second - it2->second);

        it->second = position;

        float distanceNew = glm::length(it1->second - it2->second);

        m_distance -= MOUSE_SPEED*(distanceNew - distancePrev);
    }

    RecomputeViewMatrix();

    return true;
}

bool CameraRotate::Wheel(float value)
{
    static const float WHEEL_FACTOR = 0.3f;

    m_distance += WHEEL_FACTOR * value;
    RecomputeViewMatrix();

    return true;
}

void CameraRotate::RecomputeViewMatrix()
{
    m_position = glm::vec3(
        m_distance * std::cos(m_verticalAngle) * std::sin(m_horizontalAngle),
        m_distance * std::sin(m_verticalAngle),
        m_distance * std::cos(m_verticalAngle) * std::cos(m_horizontalAngle)
    );

    m_viewMatrix = glm::lookAt(m_position, m_lookPoint, glm::vec3{ 0.0f, 1.0f, 0.0f });
}

const glm::mat4 & CameraRotate::GetViewMatrix() const
{
    return m_viewMatrix;
}

const glm::mat4 & CameraRotate::GetProjectionMatrix() const
{
    return m_projectionMatrix;
}

void CameraRotate::SetLookPoint(const glm::vec3 & lookPoint)
{
    m_lookPoint = lookPoint;

    RecomputeViewMatrix();
}

void Camera2D::Init()
{
    auto[width, height] = Common::GetWindowSize();

    // TODO 2d camera should use orthographic projection
    m_projectionMatrix = glm::perspective(m_FoV, (float)width / (float)height, GetPlanes().x, GetPlanes().y);
    RecomputeViewMatrix();
}

bool Camera2D::Press(const glm::vec2 & position, int64_t id)
{
    m_positions[id] = position;

    return true;
}

bool Camera2D::Release(const glm::vec2 & position, int64_t id)
{
    m_positions.erase(id);

    return true;
}

bool Camera2D::Move(const glm::vec2 & position, int64_t id)
{
    auto it = m_positions.find(id);
    if (it == std::end(m_positions))
        return false;

    if (m_positions.size() == 1)
    {
        glm::vec2 delta = position - it->second;
        it->second = position;

        auto[width, height] = Common::GetWindowSize();

        m_position.x -= (delta.x / width) * m_distance;
        m_position.y += (delta.y / height) * m_distance;
    }
    else if (m_positions.size() == 2)
    {
        // get other position
        auto it1 = std::begin(m_positions);
        auto it2 = std::begin(m_positions); it2++;

        if (it2 == it)
            std::swap(it1, it2);

        float distancePrev = glm::length(it1->second - it2->second);

        it->second = position;

        float distanceNew = glm::length(it1->second - it2->second);

        m_distance -= MOUSE_SPEED * (distanceNew - distancePrev);
    }

    RecomputeViewMatrix();

    return true;
}

bool Camera2D::Wheel(float value)
{
    static const float WHEEL_FACTOR = 0.1f;

    m_distance += WHEEL_FACTOR * value;
    RecomputeViewMatrix();

    return true;
}

void Camera2D::RecomputeViewMatrix()
{
    m_viewMatrix = glm::lookAt({m_position.x, m_position.y, m_distance}, m_position, glm::vec3{ 0.0f, 1.0f, 0.0f });
}

const glm::mat4 & Camera2D::GetViewMatrix() const
{
    return m_viewMatrix;
}

const glm::mat4 & Camera2D::GetProjectionMatrix() const
{
    return m_projectionMatrix;
}

const glm::vec3 & Camera2D::GetPosition() const
{
    return m_position;
}
