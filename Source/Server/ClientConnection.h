#pragma once

#include "Core/BitStream.h"
#include "Shared/ClientCommand.h"

class Server;
class SV_Player;

class ClientConnection {
public:
	ClientConnection(Server *server, ENetPeer *peer);
	~ClientConnection();

	void handleCommand(const BitStream &stream);
	void sendMessage(const BitStream &message, bool reliably);

	// Sends a message to the client to inform it of its controlled entity ID
	void sendPlayerEntityID();

private:
	void processChangeTeam(const BitStream &message);
	void processPlayerInput(const PlayerInput &input);

private:
	Server *mServer;
	ENetPeer *mPeer;
	SV_Player *mPlayer;
};