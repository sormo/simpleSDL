#pragma once
#include "glm/glm.hpp"
#include <map>
#include "EventDispatchers.h"

class Camera : public ResizeReceiver
{
public:
    virtual void Init() = 0;

    virtual const glm::mat4 & GetViewMatrix() const = 0;
    virtual const glm::mat4 & GetProjectionMatrix() const = 0;
    virtual const glm::vec3 & GetPosition() const = 0;
    virtual const glm::vec2 GetPlanes() const { return glm::vec2(0.1f, 100.0f); }
    
    float GetFoV() const { return m_FoV; }

protected:
    virtual void WindowResized(int32_t, int32_t) override;

    // Field of View in radians
    float m_FoV = 0.785f;
};

class CameraKeyboard : public MouseReceiver, public Camera
{
public:
    // we may use constructor but window size is needed in camera initialization
    void Init() override;

    void RecomputeMatrices();

    const glm::mat4 & GetViewMatrix() const override;
    const glm::mat4 & GetProjectionMatrix() const override;
    const glm::vec3 & GetPosition() const override;

    bool Wheel(float value) override;

private:
    glm::mat4 m_viewMatrix;
    glm::mat4 m_projectionMatrix;

    // Initial position : on +Z
    glm::vec3 m_position = glm::vec3(0, 0, 5);
    // Initial horizontal angle : toward -Z
    float m_horizontalAngle = 3.14f;
    // Initial vertical angle : none
    float m_verticalAngle = 0.0f;
};

// this camera rotates around center 0, 0, 0 point
class CameraRotate : public MouseReceiver, public Camera
{
public:
    // we may use constructor but window size is needed in camera initialization
    void Init() override;

    // inputs provided by application
    bool Press(const glm::vec2 & position, int64_t id) override;
    bool Release(const glm::vec2 &  position, int64_t id) override;
    bool Move(const glm::vec2 &  position, int64_t id) override;
    bool Wheel(float value) override;

    const glm::mat4 & GetViewMatrix() const override;
    const glm::mat4 & GetProjectionMatrix() const override;
    const glm::vec3 & GetPosition() const override;

    // set point at which camera is looking
    void SetLookPoint(const glm::vec3 & lookPoint);

private:
    void RecomputeViewMatrix();

    glm::mat4 m_viewMatrix;
    glm::mat4 m_projectionMatrix;

    std::map<int64_t, glm::vec2> m_positions;

    float m_distance = 5.0f;
    float m_horizontalAngle = 0.0f;
    float m_verticalAngle = 0.0f;

    glm::vec3 m_position;
    glm::vec3 m_lookPoint;
};

class Camera2D : public MouseReceiver, public Camera
{
public:
    // we may use constructor but window size is needed in camera initialization
    void Init() override;

    // inputs provided by application
    bool Press(const glm::vec2 & position, int64_t id) override;
    bool Release(const glm::vec2 &  position, int64_t id) override;
    bool Move(const glm::vec2 &  position, int64_t id) override;
    bool Wheel(float value) override;

    const glm::mat4 & GetViewMatrix() const override;
    const glm::mat4 & GetProjectionMatrix() const override;
    const glm::vec3 & GetPosition() const override;

private:
    void RecomputeViewMatrix();

    glm::mat4 m_viewMatrix;
    glm::mat4 m_projectionMatrix;

    std::map<int64_t, glm::vec2> m_positions;
    glm::vec3 m_position;

    float m_distance = 5.0f;
};
