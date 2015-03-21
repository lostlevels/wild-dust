#pragma once

#include "Core/Clock.h"
#include "Core/BitStream.h"

class ServerWorld;
class ClientConnection;

class SERVER_API Server {
public:
	Server();
	~Server();

	bool init(int portNumber, int tickRate, int sendRate, int maxPlayers);
	void shutdown();
	void update();

	ServerWorld *getWorld() { return mWorld; }

	int getPortNumber() const { return mPortNumber; }
	int getTickRate() const { return mTickRate; }
	int getSendRate() const { return mSendRate; }
	int getMaxPlayers() const { return mMaxPlayers; }

	int getNumConnectedPlayers() const { return mConnections.size(); }

private:
	void tick(float dt);
	void sendWorldUpdates();

	void processNetworkEvents();
	void handleConnectEvent(ENetPeer *peer);
	void handleRejectConnectEvent(ENetPeer *peer);
	void handleDisconnectEvent(ENetPeer *peer);
	void handleReceiveEvent(ENetPeer *peer, const BitStream &stream);

private:
	int mPortNumber;
	int mTickRate;
	int mSendRate;
	int mMaxPlayers;
	Clock mTickTock;
	float mTimeLeftToSimulate;
	Clock mSendClock;

	ServerWorld *mWorld;
	b2World *mPhysicsWorld;

private:
	ENetHost *mHost;
	std::vector<ClientConnection*> mConnections;
};
