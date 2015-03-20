#pragma once

#include "Core/BitStream.h"

class ClientConnection {
public:
	ClientConnection(ENetPeer *peer);
	~ClientConnection();

	void handleCommand(const BitStream &stream);

private:
	ENetPeer *mPeer;
};