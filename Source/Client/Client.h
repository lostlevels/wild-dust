#pragma once

#include <SDL.h>

class CLIENT_API Client {
public:
	bool init();
	void shutdown();
	void tick();

	bool isQuitSignaled() const;

private:
	SDL_Window *mGameWindow;
	SDL_GLContext mGraphicsContext;
	bool mQuitSignaled;
};
