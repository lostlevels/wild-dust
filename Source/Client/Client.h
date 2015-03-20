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
	int getWindowWidth() const { return mWindowWidth; }
	int getWindowHeight() const { return mWindowHeight; }
	bool isQuitSignaled() const { return mQuitSignaled; }

private:
	SDL_Window *mGameWindow;
	int mWindowWidth;
	int mWindowHeight;
	bool mQuitSignaled;
	Renderer *mRenderer;
};
