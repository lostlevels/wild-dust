#include "Precompiled.h"
#include "Client.h"
#include "Renderer.h"

Client::Client() {
	mQuitSignaled = false;

	mRenderer = new Renderer(this);
}

Client::~Client() {
	delete mRenderer;
}

bool Client::init() {
	SDL_Init(SDL_INIT_VIDEO);

	mWindowWidth = 1280;
	mWindowHeight = 720;

	mGameWindow = SDL_CreateWindow(
		"Code a Game 2015",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		mWindowWidth,
		mWindowHeight,
		SDL_WINDOW_OPENGL);

	if (mGameWindow == NULL)
		return false;

	if (!mRenderer->init())
		return false;

	return true;
}

void Client::shutdown() {
	mRenderer->shutdown();

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

	mRenderer->beginFrame();

	mRenderer->endFrame();
}
