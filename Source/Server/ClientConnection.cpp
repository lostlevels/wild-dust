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
	enet_uint32 packetFlags = ENET_PACKET_FLAG_NO_ALLOCATE;
	if (reliably) {
		packetFlags |= ENET_PACKET_FLAG_RELIABLE;
	}

	ENetPacket *packet = enet_packet_create(message.getDataBuffer(), message.getSize(), packetFlags);
	enet_peer_send(mPeer, 0, packet);
}

void ClientConnection::processPlayerInput(const PlayerInput &input) {
	if (input.buttonMask & BTN_MOVE_LEFT) {
		mPlayer->mPosition -= Vec2(20.0f, 0.0f);
	}
	if (input.buttonMask & BTN_MOVE_RIGHT) {
		mPlayer->mPosition += Vec2(20.0f, 0.0f);
	}
	if (input.buttonMask & BTN_JUMP) {
		mPlayer->mPosition -= Vec2(0.0f, 20.0f);
	}
}
