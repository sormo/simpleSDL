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
    bool Dispatch(const SDL_Event & event);
private:
    bool DispatchPress(const glm::vec2 & position, int64_t id);
    bool DispatchRelease(const glm::vec2 & position, int64_t id);
    bool DispatchMove(const glm::vec2 & position, int64_t id);
    bool DispatchWheel(float value);

    std::vector<MouseReceiver*> m_receivers;
};
