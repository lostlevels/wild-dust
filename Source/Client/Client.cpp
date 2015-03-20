#include "Precompiled.h"
#include "Client.h"

bool Client::init() {
	SDL_Init(SDL_INIT_VIDEO);

	mGameWindow = SDL_CreateWindow(
		"Code a Game 2015",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		1280,
		720,
		SDL_WINDOW_OPENGL);

	if (mGameWindow == NULL)
		return false;

	mGraphicsContext = SDL_GL_CreateContext(mGameWindow);
	SDL_GL_MakeCurrent(mGameWindow, mGraphicsContext);

	return true;
}

void Client::shutdown() {
	SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

void Client::tick() {
	SDL_Event evt;
	while (SDL_PollEvent(&evt)) {
		if (evt.type == SDL_QUIT) {
			mQuitSignaled = true;
			break;
		}
	}

	SDL_GL_SwapWindow(mGameWindow);
}

bool Client::isQuitSignaled() const {
	return mQuitSignaled;
}
