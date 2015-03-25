#include "Precompiled.h"
#include "ClientConnection.h"
#include "Shared/Protocol.h"
#include "Server.h"
#include "World.h"
#include "SV_Player.h"
#include "Physics/PlayerMovement.h"
#include "Map.h"

ClientConnection::ClientConnection(Server *server, ENetPeer *peer) {
	mServer = server;
	mPeer = peer;

	mPlayer = server->getWorld()->spawnEntityTyped<SV_Player>(ENTITY_PLAYER);
}

ClientConnection::~ClientConnection() {
	mServer->getWorld()->destroyEntity(mPlayer);
}

void ClientConnection::handleCommand(const BitStream &stream) {
	uint8_t cmdID = stream.readU8();
	switch (cmdID) {
	case CLIENT_CMD_INPUT:
		processPlayerInput(stream.readAny<PlayerInput>());
		break;

	case CLIENT_CMD_CHANGE_TEAM:
		processChangeTeam(stream);
		break;
	}
}

void ClientConnection::sendMessage(const BitStream &message, bool reliably) {
	enet_uint32 packetFlags = 0;
	if (reliably) {
		packetFlags |= ENET_PACKET_FLAG_RELIABLE;
	}
	else {
		packetFlags |= ENET_PACKET_FLAG_UNSEQUENCED;
	}

	ENetPacket *packet = enet_packet_create(message.getDataBuffer(), message.getSize(), packetFlags);
	enet_peer_send(mPeer, reliably ? 0 : 1, packet);
}

void ClientConnection::sendPlayerEntityID() {
	uint8_t messageBuffer[2048];
	BitStream message(messageBuffer, sizeof(messageBuffer));
	message.writeU8(NETCMD_STC_PLAYER_IDENTIFY);
	message.writeAny<EntityID>(mPlayer->getEntityID());
	sendMessage(message, true);
}

void ClientConnection::processChangeTeam(const BitStream &stream) {
	Team team = (Team)stream.readU8();
	mPlayer->changeTeam(team);
	if (team == TEAM_BANDITS) {
		mPlayer->getPhysicsObject()->setPosition(mServer->getMap()->getBSpawn());
	}
	else {
		mPlayer->getPhysicsObject()->setPosition(mServer->getMap()->getCSpawn());
	}
}

void ClientConnection::processPlayerInput(const PlayerInput &input) {
	mPlayer->processInput(input);
}
