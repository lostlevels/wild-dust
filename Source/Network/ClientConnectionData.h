#pragma once

#include <string>
#include <enet/enet.h>
#include "Core/Clock.h"

struct ClientConnectionData {
	std::string          mName;
	ENetPeer             *mPeer;
	Clock                mPingClock;
	std::vector<float>   mPings;
	float                mPing;
	bool                 mPinged;

	ClientConnectionData(std::string &name, ENetPeer *peer) : mName(name), mPeer(peer), mPing(0), mPinged(false) {}
};
