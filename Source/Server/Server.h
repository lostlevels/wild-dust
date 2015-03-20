#pragma once

#include <enet/enet.h>
#include "Core/Clock.h"

class ServerWorld;

class SERVER_API Server {
public:
	Server();
	~Server();

	bool init(int tickRate, int sendRate);
	void shutdown();
	void update();

private:
	void tick(float dt);
	void sendWorldUpdates();

	void processNetworkEvents();
	void handleConnectEvent();
	void handleDisconnectEvent();
	void handleReceiveEvent();

private:
	int mTickRate;
	int mSendRate;
	Clock mTickTock;
	float mTimeLeftToSimulate;
	Clock mSendClock;

	ServerWorld *mWorld;

private:
	ENetHost *mHost;
};
