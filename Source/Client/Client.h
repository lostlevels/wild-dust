#pragma once

class Renderer;

class CLIENT_API Client {
public:
	Client();
	~Client();

	bool init();
	void shutdown();
	void tick();

	SDL_Window *getGameWindow() const { return mGameWindow; }
	bool isQuitSignaled() const { return mQuitSignaled; }

private:
	SDL_Window *mGameWindow;
	bool mQuitSignaled;
	Renderer *mRenderer;
};
