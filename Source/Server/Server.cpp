#include "Precompiled.h"
#include "Server.h"

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
		tick();
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
			handleConnectEvent();
			break;

		case ENET_EVENT_TYPE_DISCONNECT:
			handleDisconnectEvent();
			break;

		case ENET_EVENT_TYPE_RECEIVE:
			handleReceiveEvent();
			break;
		}
	}
}

void Server::handleConnectEvent() {
	gLogger.info("Client connected!\n");
}

void Server::handleDisconnectEvent() {
	gLogger.info("Client disconnected!\n");
}

void Server::handleReceiveEvent() {

}

void Server::tick() {

}

void Server::sendWorldUpdates() {

}
