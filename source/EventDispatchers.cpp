#include "EventDispatchers.h"
#include "Common.h"

void MouseDispatcher::Add(MouseReceiver * receiver)
{
    m_receivers.push_back(receiver);
}

void MouseDispatcher::Remove(MouseReceiver* receiver)
{
    for (auto it = std::begin(m_receivers); it != std::end(m_receivers); it++)
    {
        if (*it == receiver)
        {
            m_receivers.erase(it);
            return;
        }
    }
}

bool MouseDispatcher::Dispatch(const SDL_Event & event)
{
    // touch events
    if (event.type == SDL_FINGERDOWN)
    {
        auto[width, height] = Common::GetWindowSize();
        return DispatchPress({ event.tfinger.x * (float)width, event.tfinger.y * (float)height }, event.tfinger.fingerId);
    }
    else if (event.type == SDL_FINGERUP)
    {
        auto[width, height] = Common::GetWindowSize();
        return DispatchRelease({ event.tfinger.x * (float)width, event.tfinger.y * (float)height }, event.tfinger.fingerId);
    }
    else if (event.type == SDL_FINGERMOTION)
    {
        auto[width, height] = Common::GetWindowSize();
        return DispatchMove({ event.tfinger.x * (float)width, event.tfinger.y * (float)height }, event.tfinger.fingerId);
    }
#if !defined(__ANDROID__)
    // looks like mouse events are sent also on android for some reason
    else if (event.type == SDL_MOUSEWHEEL)
    {
        //g_camera.ModifyFoV(event.wheel.y);
        return DispatchWheel((float)event.wheel.y);
    }
    else if (event.type == SDL_MOUSEBUTTONDOWN)
    {
        return DispatchPress({ (float)event.button.x, (float)event.button.y }, 0);
    }
    else if (event.type == SDL_MOUSEBUTTONUP)
    {
        return DispatchRelease({ (float)event.button.x, (float)event.button.y }, 0);
    }
    else if (event.type == SDL_MOUSEMOTION)
    {
        return DispatchMove({ (float)event.motion.x, (float)event.motion.y }, 0);
    }
#endif
    return false;
}

bool MouseDispatcher::DispatchPress(const glm::vec2 & position, int64_t id)
{
    for (MouseReceiver * receiver : m_receivers)
    {
        if (receiver->Press(position, id))
            return true;
    }
    return false;
}

bool MouseDispatcher::DispatchRelease(const glm::vec2 & position, int64_t id)
{
    for (MouseReceiver * receiver : m_receivers)
    {
        if (receiver->Release(position, id))
            return true;
    }
    return false;
}

bool MouseDispatcher::DispatchMove(const glm::vec2 & position, int64_t id)
{
    for (MouseReceiver * receiver : m_receivers)
    {
        if (receiver->Move(position, id))
            return true;
    }
    return false;
}

bool MouseDispatcher::DispatchWheel(float value)
{
    for (MouseReceiver * receiver : m_receivers)
    {
        if (receiver->Wheel(value))
            return true;
    }
    return false;
}

///////////////////////////////////////////////////////////////////////////////

void ResizeDispatcher::Add(ResizeReceiver* receiver)
{
    m_receivers.push_back(receiver);
}

void ResizeDispatcher::Remove(ResizeReceiver* receiver)
{
    for (auto it = std::begin(m_receivers); it != std::end(m_receivers); it++)
    {
        if (*it == receiver)
        {
            m_receivers.erase(it);
            return;
        }
    }
}

bool ResizeDispatcher::Dispatch(const SDL_Event& event)
{
#if !defined(__ANDROID__)
    if (event.type == SDL_WINDOWEVENT)
    {
        if (event.window.event == SDL_WINDOWEVENT_RESIZED)
        {
            auto [width, height] = Common::GetWindowSize();

            for (auto receiver : m_receivers)
                receiver->WindowResized(width, height);
        }
    }
#endif

    return false;
}
