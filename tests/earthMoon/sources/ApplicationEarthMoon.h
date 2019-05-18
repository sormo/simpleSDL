#pragma once
#include "Application.h"

class ApplicationEarthMoon : public Application
{
public:
    virtual bool Init() override;
    virtual bool MainLoop() override;
    virtual void Deinit() override;
    virtual void Gui() override;

    virtual void Dispatch(const SDL_Event & event) override;
};
