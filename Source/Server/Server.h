#pragma once

#include "Core/Clock.h"
#include "Core/BitStream.h"

class ServerWorld;
class ClientConnection;

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
	void handleConnectEvent(ENetPeer *peer);
	void handleDisconnectEvent(ENetPeer *peer);
	void handleReceiveEvent(ENetPeer *peer, const BitStream &stream);

private:
	int mTickRate;
	int mSendRate;
	Clock mTickTock;
	float mTimeLeftToSimulate;
	Clock mSendClock;

	ServerWorld *mWorld;

private:
	ENetHost *mHost;

	std::vector<ClientConnection*> mConnections;
};
