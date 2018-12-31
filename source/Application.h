#pragma once
#include <SDL.h>

namespace Application
{
	bool Init();
	bool MainLoop();
	void Deinit();

    void Dispatch(const SDL_Event & event);
}
