#pragma once

#include <enet/enet.h>
#include "Core/Clock.h"

class SERVER_API Server {
public:
	bool init(int tickRate, int sendRate);
	void shutdown();
	void update();

private:
	void tick();
	void sendWorldUpdates();

private:
	int mTickRate;
	int mSendRate;

	Clock mTickTock;

	float mTimeLeftToSimulate;

	Clock mSendClock;
};
