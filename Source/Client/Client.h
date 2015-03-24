#pragma once

#include "Core/Clock.h"
#include "Core/BitStream.h"
#include "Core/Config.h"
#include "Shared/Protocol.h"

class Physics;
class Renderer;
class ClientWorld;
class ClientMap;
class InputSystem;
class AudioSystem;
class GUI;
class Camera;
class CL_Player;
class Music;

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

	Physics *getPhysics() const { return mPhysics; }
	Renderer *getRenderer() const { return mRenderer; }
	GUI *getGUI() const { return mGUI; }

	ClientNetworkState getNetworkState() { return mNetworkState; }

	Config getSettings() const { return mSettings; }

	void addSky();
	
private:
	void sendPlayerInput();
	void handleInput(PlayerInput input);
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
	Physics *mPhysics;
	Renderer *mRenderer;
	ClientWorld *mWorld;
	ClientMap *mMap;
	InputSystem *mInput;
	AudioSystem *mAudio;
	GUI *mGUI;
	Camera *mCamera;
	Music *mMusic;

private:
	ENetHost *mHost;
	ENetPeer *mPeer;
	ClientNetworkState mNetworkState;
	Clock mConnectionClock;
	Clock mSendInputClock;
	float mPhysicsAccum;
	CL_Player *mPlayerEntity;
};
