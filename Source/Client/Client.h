#pragma once

#include "Core/Clock.h"
#include "Core/BitStream.h"
#include "Core/Config.h"

class Renderer;
class ClientWorld;
class ClientMap;
class InputSystem;
class AudioSystem;
class GUI;
class Camera;
class CL_Player;

enum ClientNetworkState {
	CLIENT_IDLE,
	CLIENT_CONNECTING,
	CLIENT_CONNECTED,
	CLIENT_INGAME
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

	Renderer *getRenderer() const { return mRenderer; }
	GUI *getGUI() const { return mGUI; }

	ClientNetworkState getNetworkState() { return mNetworkState; }

private:
	void sendPlayerInput();
	void processNetworkEvents();
	void handleConnectEvent();
	void handleDisconnectEvent();
	void handleReceiveEvent(const BitStream &stream);
	void handleIdentifyPlayerCommand(const BitStream &stream);
	void handleLoadMapCommand(const BitStream &stream);

	void renderFrame();

private:
	SDL_Window *mGameWindow;
	int mWindowWidth;
	int mWindowHeight;
	bool mQuitSignaled;
	Clock mTickTock;
	Config mSettings;
	Renderer *mRenderer;
	ClientWorld *mWorld;
	ClientMap *mMap;
	InputSystem *mInput;
	AudioSystem *mAudio;
	GUI *mGUI;
	Camera *mCamera;

private:
	ENetHost *mHost;
	ENetPeer *mPeer;
	ClientNetworkState mNetworkState;
	Clock mConnectionClock;
	Clock mSendInputClock;
	CL_Player *mPlayerEntity;
};
