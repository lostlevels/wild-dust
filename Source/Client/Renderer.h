#pragma once

class Client;

class Renderer {
public:
	Renderer(Client *context);

	bool init();
	void shutdown();

	void beginFrame();
	void endFrame();

private:
	Client *mContext;
	SDL_GLContext mGLContext;
};
