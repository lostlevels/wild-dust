#pragma once

#include "Core/Clock.h"

class Renderer;

enum ClientNetworkState {
	CLIENT_IDLE,
	CLIENT_CONNECTING,
	CLIENT_CONNECTED,
};

class CLIENT_API Client {
public:
	Client();
	~Client();

	bool init();
	void shutdown();

	void connectToServer(const char *hostName, int serverPort);
	void disconnectFromServer();

	void tick();

	SDL_Window *getGameWindow() const { return mGameWindow; }
	int getWindowWidth() const { return mWindowWidth; }
	int getWindowHeight() const { return mWindowHeight; }
	bool isQuitSignaled() const { return mQuitSignaled; }

private:
	void processNetworkEvents();
	void handleConnectEvent();
	void handleDisconnectEvent();
	void handleReceiveEvent();

	void renderFrame();

private:
	SDL_Window *mGameWindow;
	int mWindowWidth;
	int mWindowHeight;
	bool mQuitSignaled;
	Renderer *mRenderer;
	Clock mTickTock;

private:
	ENetHost *mHost;
	ENetPeer *mPeer;
	ClientNetworkState mNetworkState;
	Clock mConnectionClock;
};
