#pragma once

#include "Core/BitStream.h"
#include "Shared/Protocol.h"

class Server;
class SV_Player;

class ClientConnection {
public:
	ClientConnection(Server *server, ENetPeer *peer);
	~ClientConnection();

	void handleCommand(const BitStream &stream);
	void sendMessage(const BitStream &message, bool reliably);

private:
	void processPlayerInput(const PlayerInput &input);

private:
	Server *mServer;
	ENetPeer *mPeer;
	SV_Player *mPlayer;
};