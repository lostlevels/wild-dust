#include "Precompiled.h"
#include "SDLContext.h"

bool SDLContext::init(const Config &settings) {
	SDL_Init(SDL_INIT_VIDEO);
	TTF_Init();

	mWindowWidth = settings.getInt("ResolutionWidth", 640);
	mWindowHeight = settings.getInt("ResolutionHeight", 480);

	Uint32 windowFlags = SDL_WINDOW_OPENGL;
	if (settings.getBool("Fullscreen", false)) {
		windowFlags |= SDL_WINDOW_FULLSCREEN;
	}

	mGameWindow = SDL_CreateWindow(
		"Code a Game 2015",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		mWindowWidth,
		mWindowHeight,
		windowFlags);

	mQuitSignaled = false;

	return mGameWindow != nullptr;
}

void SDLContext::tick() {
	SDL_Event evt;
	while (SDL_PollEvent(&evt)) {
		if (evt.type == SDL_QUIT) {
			mQuitSignaled = true;
			break;
		}

		for (auto &handler : mHandlers) {
			if (handler->handle(evt)) break;
		}
	}
}

void SDLContext::registerHandler(SDLEventHandler *handler) {
	removeHandler(handler);
	mHandlers.push_back(handler);
}

void SDLContext::removeHandler(SDLEventHandler *handler) {
	auto result = std::find(std::begin(mHandlers), std::end(mHandlers), handler);
	if (result != mHandlers.end())
		mHandlers.erase(result);
}