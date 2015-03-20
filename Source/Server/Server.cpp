#include "Precompiled.h"
#include "Server.h"
#include "World.h"
#include "ClientConnection.h"
#include "Shared/Protocol.h"
#include "EntityRegistration.h"

Server::Server() {
	mWorld = new ServerWorld(this);
	RegisterServerEntityTypes(mWorld);

	b2Vec2 gravity(0.0f, 100.0f);
	mPhysicsWorld = new b2World(gravity);
}

Server::~Server() {
	delete mPhysicsWorld;
	delete mWorld;
}

bool Server::init(int tickRate, int sendRate) {
	mTickRate = tickRate;
	mSendRate = sendRate;
	
	ENetAddress hostAddr;
	hostAddr.host = ENET_HOST_ANY;
	hostAddr.port = 5000;

	mHost = enet_host_create(&hostAddr, 32, 2, 0, 0);
	if (mHost == NULL) {
		gLogger.error("Could not create ENet host.\n");
		return false;
	}

	mTimeLeftToSimulate = 0.0f;

	return true;
}

void Server::shutdown() {
	for (ClientConnection *conn : mConnections) {
		delete conn;
	}
	mConnections.clear();

	enet_host_destroy(mHost);
}

void Server::update() {
	float dt = mTickTock.getElapsedSeconds();
	mTickTock.reset();

	// Simulates the world by a fixed timestep as much as necessary
	mTimeLeftToSimulate += dt;
	const  float tickTimestep = 1.0f / (float)mTickRate;
	while (mTimeLeftToSimulate >= tickTimestep) {
		tick(tickTimestep);
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
			handleConnectEvent(evt.peer);
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

	peer->data = connection;

	char ipAddress[128];
	enet_address_get_host_ip(&peer->address, ipAddress, sizeof(ipAddress));

	gLogger.info("Client connected from %s:%d!\n", ipAddress, peer->address.port);
}

void Server::handleDisconnectEvent(ENetPeer *peer) {
	for (auto it = mConnections.begin(); it != mConnections.end(); ++it) {
		if (*it == peer->data) {
			delete *it;
			mConnections.erase(it);
			break;
		}
	}

	gLogger.info("Client disconnected!\n");
}

void Server::handleReceiveEvent(ENetPeer *peer, const BitStream &stream) {
	ClientConnection *conn = reinterpret_cast<ClientConnection*>(peer->data);
	conn->handleCommand(stream);
}

void Server::tick(float dt) {
	mPhysicsWorld->Step(dt, 10, 10);
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
