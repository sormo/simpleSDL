#pragma once
#include "glm/glm.hpp"
#include <SDL.h>
#include <vector>

class MouseReceiver
{
public:
    virtual bool Press(const glm::vec2 & position, int64_t id) { return false; }
    virtual bool Release(const glm::vec2 &  position, int64_t id) { return false; }
    virtual bool Move(const glm::vec2 &  position, int64_t id) { return false; }
    virtual bool Wheel(float value) { return false; }
};

class MouseDispatcher
{
public:
    void Add(MouseReceiver * receiver);
    void Remove(MouseReceiver* receiver);
    bool Dispatch(const SDL_Event & event);
private:
    bool DispatchPress(const glm::vec2 & position, int64_t id);
    bool DispatchRelease(const glm::vec2 & position, int64_t id);
    bool DispatchMove(const glm::vec2 & position, int64_t id);
    bool DispatchWheel(float value);

    std::vector<MouseReceiver*> m_receivers;
};

///////////////////////////////////////////////////////////////////////////////

class ResizeReceiver
{
public:
    virtual void WindowResized(int32_t width, int32_t height) = 0;
};

class ResizeDispatcher
{
public:
    void Add(ResizeReceiver* receiver);
    void Remove(ResizeReceiver* receiver);
    bool Dispatch(const SDL_Event& event);
private:
    std::vector<ResizeReceiver*> m_receivers;

};
