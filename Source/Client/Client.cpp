#include "Precompiled.h"
#include "Client.h"
#include "Renderer.h"
#include "World.h"
#include "Map.h"
#include "Shared/Protocol.h"
#include "EntityRegistration.h"
#include "Input.h"
#include "Audio.h"
#include "GUI.h"
#include "Camera.h"
#include "Player.h"
#include <SDL_ttf.h>

Client::Client() {
	mQuitSignaled = false;

	mRenderer = new Renderer(this);

	mWorld = new ClientWorld(this);
	RegisterClientEntityTypes(mWorld);

	mMap = new ClientMap(this);

	mInput = new InputSystem(this);

	mAudio = new AudioSystem();

	mGUI = new GUI(this);

	mCamera = new Camera();

	mHost = enet_host_create(NULL, 1, 2, 0, 0);
	if (mHost == NULL) {
		gLogger.error("Could not create client host!\n");
	}

	mPeer = NULL;
	mPlayerEntity = NULL;
}

Client::~Client() {
	enet_host_destroy(mHost);
	delete mCamera;
	delete mGUI;
	delete mAudio;
	delete mInput;
	delete mMap;
	delete mWorld;
	delete mRenderer;
}

bool Client::init() {
	SDL_Init(SDL_INIT_VIDEO);
	TTF_Init();

	mSettings.loadFromFile("Config/Game.cfg");

	mWindowWidth = mSettings.getInt("ResolutionWidth", 640);
	mWindowHeight = mSettings.getInt("ResolutionHeight", 480);

	Uint32 windowFlags = SDL_WINDOW_OPENGL;
	if (mSettings.getBool("Fullscreen", false)) {
		windowFlags |= SDL_WINDOW_FULLSCREEN;
	}

	mGameWindow = SDL_CreateWindow(
		"Code a Game 2015",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		mWindowWidth,
		mWindowHeight,
		windowFlags);

	if (mGameWindow == NULL)
		return false;
    
    bool rendererInited = mRenderer->init();
    assert(rendererInited);
    
	if (!rendererInited)
		return false;

	mAudio->init();

	if (!mGUI->init())
		return false;

	mTickTock.reset();

	mNetworkState = CLIENT_IDLE;

	return true;
}

void Client::shutdown() {
	mMap->unload();

	mWorld->deleteAllEntities();

	mGUI->shutdown();

	mAudio->shutdown();

	mRenderer->shutdown();

	disconnectFromServer();

	TTF_Quit();
	SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

void Client::connectToServer(const char *hostName, int serverPort) {
	disconnectFromServer();

	ENetAddress finalAddr;
	enet_address_set_host(&finalAddr, hostName);
	finalAddr.port = serverPort;

	mPeer = enet_host_connect(mHost, &finalAddr, 2, 0);
	if (mPeer == NULL) {
		gLogger.error("Connection to server failed.\n");
		return;
	}

	mNetworkState = CLIENT_CONNECTING;
	mConnectionClock.reset();

	gLogger.info("Connection to server initiated..\n");
}

void Client::disconnectFromServer() {
	if (mPeer != NULL) {
		enet_peer_disconnect_now(mPeer, 0);
		mPeer = NULL;
	}
	mNetworkState = CLIENT_IDLE;
	mPlayerEntity = NULL;
}

void Client::tick() {
	SDL_Event evt;
	while (SDL_PollEvent(&evt)) {
		if (evt.type == SDL_QUIT) {
			mQuitSignaled = true;
			break;
		}
	}

	float dt = mTickTock.getElapsedSeconds();
	mTickTock.reset();

	// TODO: Don't hardcode this..
	if (mSendInputClock.getElapsedSeconds() >= (1.0f / 128.0f)) {
		sendPlayerInput();
		mSendInputClock.reset();
	}

	processNetworkEvents();

	if (mPlayerEntity) {
		mCamera->setTarget(mPlayerEntity);
	}
	mCamera->setScreenSize(mWindowWidth, mWindowHeight);
	mCamera->update();

	mWorld->update(dt);

	renderFrame();

	mAudio->update();
}

void Client::sendPlayerInput() {
	if (mNetworkState < CLIENT_INGAME) {
		return;
	}

	PlayerInput input = mInput->makeInputState();

	uint8_t messageBuffer[128000];
	BitStream message(messageBuffer, sizeof(messageBuffer));
	message.writeU8(NETCMD_CTS_PLAYER_INPUT);
	message.writeAny<PlayerInput>(input);

	ENetPacket *packet = enet_packet_create(message.getDataBuffer(), message.getSize(), ENET_PACKET_FLAG_RELIABLE | ENET_PACKET_FLAG_NO_ALLOCATE);
	enet_peer_send(mPeer, 0, packet);
}

void Client::processNetworkEvents() {
	ENetEvent evt;
	while (enet_host_service(mHost, &evt, 0)) {
		switch (evt.type) {
		case ENET_EVENT_TYPE_CONNECT:
			handleConnectEvent();
			break;

		case ENET_EVENT_TYPE_DISCONNECT:
			handleDisconnectEvent();
			break;

		case ENET_EVENT_TYPE_RECEIVE:
			BitStream stream(evt.packet->data, evt.packet->dataLength);
			handleReceiveEvent(stream);
			enet_packet_destroy(evt.packet);
			break;
		}
	}

	if (mNetworkState == CLIENT_CONNECTING && mConnectionClock.getElapsedSeconds() >= 5.0f) {
		gLogger.info("Connection to server failed after 5 seconds, aborting.\n");
		disconnectFromServer();
	}
}

void Client::handleConnectEvent() {
	gLogger.info("Connection to server succeeded.\n");
	mNetworkState = CLIENT_CONNECTED;
}

void Client::handleDisconnectEvent() {
	gLogger.info("Disconnected from server.\n");
	mNetworkState = CLIENT_IDLE;
	mPeer = NULL;
}

void Client::handleReceiveEvent(const BitStream &stream) {
	uint8_t cmdID = stream.readU8();
	switch (cmdID) {
	case NETCMD_STC_PLAYER_IDENTIFY:
		handleIdentifyPlayerCommand(stream);
		break;

	case NETCMD_STC_LOAD_MAP:
		handleLoadMapCommand(stream);
		break;

	case NETCMD_STC_WORLD_SNAPSHOT:
		mWorld->readFromSnapshot(stream);
		break;
	}
}

void Client::handleIdentifyPlayerCommand(const BitStream &stream) {
	EntityID id = stream.readAny<EntityID>();
	mPlayerEntity = (CL_Player*)mWorld->findEntityByID(id);
}

void Client::handleLoadMapCommand(const BitStream &stream) {
	std::string mapName = stream.readString();

	if (!mMap->loadFromFile(mapName)) {
		gLogger.error("Could not map, disconnecting.\n");
		disconnectFromServer();
		return;
	}

	mRenderer->freeUnreferencedTextures();

	mNetworkState = CLIENT_INGAME;
}

void Client::renderFrame() {
	mRenderer->beginFrame(mCamera);
	mMap->draw();
	mWorld->draw();
	mRenderer->endFrame();
}
