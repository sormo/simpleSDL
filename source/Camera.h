#pragma once
#include "glm/glm.hpp"
#include <map>

// TODO maybe inheritance from common Camera base

class CameraKeyboard
{
public:
    // we may use constructor but window size is needed in camera initialization
    void Init();

    void RecomputeMatrices();
    const glm::mat4 & GetViewMatrix();
    const glm::mat4 & GetProjectionMatrix();

    const glm::vec3 & GetPosition();

    void ModifyFoV(float value);

private:
    glm::mat4 m_viewMatrix;
    glm::mat4 m_projectionMatrix;

    // Initial position : on +Z
    glm::vec3 m_position = glm::vec3(0, 0, 5);
    // Initial horizontal angle : toward -Z
    float m_horizontalAngle = 3.14f;
    // Initial vertical angle : none
    float m_verticalAngle = 0.0f;
    // Initial Field of View
    float m_FoV = 0.785f;
};

// this camera rotates around center 0, 0, 0 point
class CameraRotate
{
public:
    // we may use constructor but window size is needed in camera initialization
    void Init();

    // inputs provided by application
    void Press(const glm::vec2 & position, int64_t id);
    void Release(const glm::vec2 &  position, int64_t id);
    void Move(const glm::vec2 &  position, int64_t id);
    void Wheel(float value);

    const glm::mat4 & GetViewMatrix();
    const glm::mat4 & GetProjectionMatrix();

    const glm::vec3 & GetPosition();

private:
    void RecomputeViewMatrix();

    glm::mat4 m_viewMatrix;
    glm::mat4 m_projectionMatrix;

    std::map<int64_t, glm::vec2> m_positions;

    float m_distance = 5.0f;
    float m_horizontalAngle = 0.0f;
    float m_verticalAngle = 0.0f;

    glm::vec3 m_position;
};

class Camera2D
{
public:
    // we may use constructor but window size is needed in camera initialization
    void Init();

    // inputs provided by application
    void Press(const glm::vec2 & position, int64_t id);
    void Release(const glm::vec2 &  position, int64_t id);
    void Move(const glm::vec2 &  position, int64_t id);
    void Wheel(float value);

    const glm::mat4 & GetViewMatrix();
    const glm::mat4 & GetProjectionMatrix();
private:
    void RecomputeViewMatrix();

    glm::mat4 m_viewMatrix;
    glm::mat4 m_projectionMatrix;

    std::map<int64_t, glm::vec2> m_positions;
    glm::vec3 m_position;

    float m_distance = 5.0f;
};
