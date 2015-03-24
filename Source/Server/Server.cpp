#include "Precompiled.h"
#include "Server.h"
#include "World.h"
#include "ClientConnection.h"
#include "Shared/Protocol.h"
#include "EntityRegistration.h"
#include "Map.h"
#include "Physics/Physics.h"

Server::Server() {
	mPhysics = new Physics();

	mWorld = new ServerWorld(this);
	RegisterServerEntityTypes(mWorld);

	mMap = new ServerMap(this);
}

Server::~Server() {
	delete mMap;
	delete mWorld;
	delete mPhysics;
}

bool Server::init(int portNumber, int tickRate, int sendRate, int maxPlayers, const std::string &mapName) {
	mPortNumber = portNumber;
	mTickRate = tickRate;
	mSendRate = sendRate;
	mMaxPlayers = maxPlayers;
	
	ENetAddress hostAddr;
	hostAddr.host = ENET_HOST_ANY;
	hostAddr.port = portNumber;

	mHost = enet_host_create(&hostAddr, 32, 2, 0, 0);
	if (mHost == NULL) {
		gLogger.error("Could not create ENet host.\n");
		return false;
	}

	changeMap(mapName);

	mTimeLeftToSimulate = 0.0f;

	gLogger.info("Server is now listening on port %d.\n", portNumber);

	return true;
}

void Server::shutdown() {
	gLogger.info("Server is shutting down..\n");

	for (ClientConnection *conn : mConnections) {
		delete conn;
	}
	mConnections.clear();

	enet_host_destroy(mHost);
}

void Server::changeMap(const std::string &mapName) {
	if (mMap->loadFromFile(mapName) == false) {
		return;
	}

	uint8_t messageBuffer[2048];
	BitStream message(messageBuffer, sizeof(messageBuffer));
	message.writeU8(NETCMD_STC_LOAD_MAP);
	message.writeString(mapName);

	for (ClientConnection *conn : mConnections) {
		conn->sendMessage(message, true);
	}
}

void Server::update() {
	float dt = mTickTock.getElapsedSeconds();
	mTickTock.reset();

	// Simulates the world by a fixed timestep as much as necessary
	mTimeLeftToSimulate += dt;
	const  float tickTimestep = 1.0f / (float)mTickRate;
	while (mTimeLeftToSimulate >= tickTimestep) {
		tick(tickTimestep);
		mWorld->destroyScheduledEntities();
		mTimeLeftToSimulate -= tickTimestep;
	}

	// Sends out world updates to clients
	if (mSendClock.getElapsedSeconds() >= (1.0f / (float)mSendRate)) {
		sendWorldUpdates();
		mWorld->deleteRemovedEntities();
		mSendClock.reset();
	}

	// Generic ENet socket ops
	processNetworkEvents();
}

void Server::processNetworkEvents() {
	ENetEvent evt;
	while (enet_host_service(mHost, &evt, 0)) {
		switch (evt.type) {
		case ENET_EVENT_TYPE_CONNECT:
			if (getNumConnectedPlayers() >= mMaxPlayers) {
				handleRejectConnectEvent(evt.peer);
			}
			else {
				handleConnectEvent(evt.peer);
			}
			break;

		case ENET_EVENT_TYPE_DISCONNECT:
			handleDisconnectEvent(evt.peer);
			break;

		case ENET_EVENT_TYPE_RECEIVE:
			BitStream stream(evt.packet->data, evt.packet->dataLength);
			handleReceiveEvent(evt.peer, stream);
			enet_packet_destroy(evt.packet);
			break;
		}
	}
}

void Server::handleConnectEvent(ENetPeer *peer) {
	ClientConnection *connection = new ClientConnection(this, peer);
	mConnections.push_back(connection);

	if (mMap->isLoaded()) {
		uint8_t messageBuffer[2048];
		BitStream message(messageBuffer, sizeof(messageBuffer));
		message.writeU8(NETCMD_STC_LOAD_MAP);
		message.writeString(mMap->getName());
		connection->sendMessage(message, true);
	}

	// Send baseline snapshot (reliably!)
	uint8_t snapshotBuffer[128000];
	BitStream snapshot(snapshotBuffer, sizeof(snapshotBuffer));
	snapshot.writeU8(NETCMD_STC_WORLD_SNAPSHOT);
	mWorld->writeToSnapshot(snapshot);
	connection->sendMessage(snapshot, true);

	connection->sendPlayerEntityID();

	peer->data = connection;

	char ipAddress[128];
	enet_address_get_host_ip(&peer->address, ipAddress, sizeof(ipAddress));
	gLogger.info("Client connected from %s:%d.\n", ipAddress, peer->address.port);
}

void Server::handleRejectConnectEvent(ENetPeer *peer) {
	char ipAddress[128];
	enet_address_get_host_ip(&peer->address, ipAddress, sizeof(ipAddress));
	gLogger.info("Client with address %s:%d was rejected (server is full!).\n", ipAddress, peer->address.port);
}

void Server::handleDisconnectEvent(ENetPeer *peer) {
	for (auto it = mConnections.begin(); it != mConnections.end(); ++it) {
		if (*it == peer->data) {
			delete *it;
			mConnections.erase(it);
			break;
		}
	}

	char ipAddress[128];
	enet_address_get_host_ip(&peer->address, ipAddress, sizeof(ipAddress));
	gLogger.info("Client with address %s:%d disconnected.\n", ipAddress, peer->address.port);
}

void Server::handleReceiveEvent(ENetPeer *peer, const BitStream &stream) {
	ClientConnection *conn = reinterpret_cast<ClientConnection*>(peer->data);

	uint8_t numCmds = stream.readU8();

	for (uint8_t i = 0; i < numCmds; ++i) {
		conn->handleCommand(stream);
	}
}

void Server::tick(float dt) {
	mPhysics->step(dt);
	mWorld->update(dt);
}

void Server::sendWorldUpdates() {
	uint8_t snapshotBuffer[128000];

	BitStream snapshot(snapshotBuffer, sizeof(snapshotBuffer));
	snapshot.writeU8(NETCMD_STC_WORLD_SNAPSHOT);
	mWorld->writeToSnapshot(snapshot);

	for (ClientConnection *conn : mConnections) {
		conn->sendMessage(snapshot, false);
	}
}
