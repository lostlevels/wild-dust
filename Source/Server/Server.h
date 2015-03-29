#pragma once

#include "Core/Clock.h"
#include "Core/BitStream.h"

class ServerWorld;
class ClientConnection;
class ServerMap;
class Physics;

class SERVER_API Server {
public:
	Server();
	~Server();

	bool init(int portNumber, int tickRate, int sendRate, int maxPlayers, const std::string &mapName);
	void shutdown();

	void changeMap(const std::string &mapName);

	void update();

	Physics *getPhysics() { return mPhysics; }

	ServerWorld *getWorld() { return mWorld; }
	ServerMap *getMap() { return mMap; }

	int getPortNumber() const { return mPortNumber; }
	int getTickRate() const { return mTickRate; }
	int getSendRate() const { return mSendRate; }
	int getMaxPlayers() const { return mMaxPlayers; }

	int getNumConnectedPlayers() const { return mConnections.size(); }

	Clock getGameClock() const { return mGameClock; }

private:
	void tick(float dt);
	void sendWorldUpdates();

	void processNetworkEvents();
	void handleConnectEvent(ENetPeer *peer);
	void handleRejectConnectEvent(ENetPeer *peer);
	void handleDisconnectEvent(ENetPeer *peer);
	void handleReceiveEvent(ENetPeer *peer, const BitStream &stream);

private:
	Clock mGameClock;

	int mPortNumber;
	int mTickRate;
	int mSendRate;
	int mMaxPlayers;
	Clock mTickTock;
	float mTimeLeftToSimulate;
	Clock mSendClock;

	Physics *mPhysics;

	ServerWorld *mWorld;
	ServerMap *mMap;

private:
	ENetHost *mHost;
	std::vector<ClientConnection*> mConnections;
};
