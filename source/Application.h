#pragma once
#include <SDL.h>
#include "EventDispatchers.h"

class Application
{
public:
    virtual bool Init() { return true; }
    virtual bool MainLoop() = 0;
    virtual void Deinit() {}
    virtual void Gui() {}

    virtual void Dispatch(const SDL_Event&) {}

    bool Execute();

    // TODO dispatch resize events for framebuffer ... 
    static ResizeDispatcher g_resizeDispatcher;

private:
    void GuiInit();
    void GuiDeinit();
    void GuiRender();

    bool AppInit();
    void AppDeinit();

    void ProcessFrame();
    void RenderFrame();
};
