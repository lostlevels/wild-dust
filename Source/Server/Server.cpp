#include "Precompiled.h"
#include "Server.h"

bool Server::init(int tickRate, int sendRate) {
	mTickRate = tickRate;
	mSendRate = sendRate;
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
}

void Server::tick() {

}

void Server::sendWorldUpdates() {

}
