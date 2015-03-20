#include "Precompiled.h"
#include "Client.h"
#include "Renderer.h"
#include "World.h"
#include "Shared/Protocol.h"
#include "EntityRegistration.h"
#include "Input.h"

Client::Client() {
	mQuitSignaled = false;

	mRenderer = new Renderer(this);

	mWorld = new ClientWorld(this);
	RegisterClientEntityTypes(mWorld);

	mInput = new InputSystem(this);

	mHost = enet_host_create(NULL, 1, 2, 0, 0);
	if (mHost == NULL) {
		gLogger.error("Could not create client host!\n");
	}

	mPeer = NULL;
}

Client::~Client() {
	delete mInput;
	delete mWorld;
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

	mTickTock.reset();

	mNetworkState = CLIENT_IDLE;

	return true;
}

void Client::shutdown() {
	mWorld->deleteAllEntities();

	mRenderer->shutdown();

	disconnectFromServer();

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

	mWorld->update(dt);

	renderFrame();
}

void Client::sendPlayerInput() {
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
	case NETCMD_STC_WORLD_SNAPSHOT:
		mWorld->readFromSnapshot(stream);
		break;
	}
}

void Client::renderFrame() {
	mRenderer->beginFrame();

	mWorld->draw();

	mRenderer->endFrame();
}
