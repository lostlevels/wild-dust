#include "Precompiled.h"
#include "ClientConnection.h"
#include "Shared/Protocol.h"
#include "Player.h"
#include "Server.h"
#include "World.h"

ClientConnection::ClientConnection(Server *server, ENetPeer *peer) {
	mServer = server;
	mPeer = peer;

	mPlayer = server->getWorld()->spawnEntityTyped<SV_Player>("Player");
}

ClientConnection::~ClientConnection() {
	mServer->getWorld()->destroyEntity(mPlayer);
}

void ClientConnection::handleCommand(const BitStream &stream) {
	uint8_t cmdID = stream.readU8();
	switch (cmdID) {
	case NETCMD_CTS_PLAYER_INPUT:
		processPlayerInput(stream.readAny<PlayerInput>());
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

void ClientConnection::processPlayerInput(const PlayerInput &input) {
	if (input.buttonMask & BTN_MOVE_LEFT) {
		mPlayer->moveLeft();
	}
	if (input.buttonMask & BTN_MOVE_RIGHT) {
		mPlayer->moveRight();
	}
	if (input.buttonMask & BTN_JUMP) {
		mPlayer->jump();
	}
	if (input.buttonMask & BTN_ATTACK) {
		mPlayer->shoot();
	}
}
