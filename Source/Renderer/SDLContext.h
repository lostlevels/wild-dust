#pragma once

#include "Core/Config.h"

struct SDLEventHandler {
	// Return true if event handled and should stop propogating
	virtual bool handle(const SDL_Event &evt) = 0;
};

class SDLContext {
public:
	bool init(const Config &settings);
	void tick();

	// Remove is probably not needed as a context should lasts at least as long as a handler
	void registerHandler(SDLEventHandler *handler);
	void removeHandler(SDLEventHandler *handler);

	SDL_Window *getGameWindow() const { return mGameWindow; }
	int getWindowWidth() const { return mWindowWidth; }
	int getWindowHeight() const { return mWindowHeight; }
	bool isQuitSignaled() const { return mQuitSignaled; }
private:
	SDL_Window *mGameWindow;
	int mWindowWidth;
	int mWindowHeight;
	bool mQuitSignaled;

	std::vector<SDLEventHandler*> mHandlers;
};
