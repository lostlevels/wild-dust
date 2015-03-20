#include "Precompiled.h"
#include "Server.h"
#include "World.h"
#include "ClientConnection.h"

Server::Server() {
	mWorld = new ServerWorld();
}

Server::~Server() {
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


	return true;
}

void Server::shutdown() {

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
			break;
		}
	}
}

void Server::handleConnectEvent(ENetPeer *peer) {
	ClientConnection *connection = new ClientConnection(peer);
	mConnections.push_back(connection);

	peer->data = connection;

	char ipAddress[128];
	enet_address_get_host_ip(&peer->address, ipAddress, sizeof(ipAddress));

	gLogger.info("Client connected from %s:%d!\n", ipAddress, peer->address.port);
}

void Server::handleDisconnectEvent(ENetPeer *peer) {
	for (auto it = mConnections.begin(); it != mConnections.end(); ++it) {
		if (*it == peer->data) {
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
	mWorld->update(dt);
}

void Server::sendWorldUpdates() {

}
