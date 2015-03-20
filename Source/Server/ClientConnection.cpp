#include "Precompiled.h"
#include "ClientConnection.h"
#include "Shared/Protocol.h"

ClientConnection::ClientConnection(ENetPeer *peer) {
	mPeer = peer;
}

ClientConnection::~ClientConnection() {

}

void ClientConnection::handleCommand(const BitStream &stream) {
	uint8_t cmdID = stream.readU8();
	switch (cmdID) {
	case NETCMD_CTS_PLAYER_INPUT:
		// TODO: Read player input
		break;
	}
}
