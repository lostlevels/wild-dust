#pragma once

#include <string>
#include <map>
#include <enet/enet.h>
#include "Core/EventEmitter.h"

//
// Emitted events:
//
//

// All data in bitstream assumed to be:
//
// "eventName"
// data
//
//

class Connection : public EventEmitter {
public:
	Connection();

	void connect(const std::string &host, unsigned short port);
	void serve(unsigned short port);

	// receiver ignored if client => server
	void send(const uint8_t *data, int size, bool reliable, const std::string &receiver = "");
	void send(const BitStream &stream, bool reliable, const std::string &receiver = "");

	void disconnect();

	// Call each frame to update
	void processNetworkEvents();

	bool isServer() const { return mServing; }

	bool isFullyReady() const;

	const std::string &getName() const { return mPlayerName; }
	float getPing() const { return mPing; }
	
private:
	ENetHost *mHost;
	ENetPeer *mClientToServer;

	// Double map
	std::map<ENetPeer*, std::string> mClientsToNames;
	std::map<std::string, ENetPeer*> mNamesToClients;

	bool mServing;
	bool mConnected;

	// Client only
	std::string mPlayerName;
	float mPing;

	void processServer();
	void processClient();

	void onClientConnected(ENetPeer *peer);
	void onClientDisconnected(ENetPeer *peer);

	void onConnectedToServer();
	void onServerDisconnected();

	void onClientData(const BitStream &stream, ENetPeer *peer);
	void onServerData(const BitStream &stream);
	
	void sendTo(ENetPacket *packet, ENetPeer *peer);
};
