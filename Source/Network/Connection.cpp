#include "Precompiled.h"
#include "Connection.h"
#include "Core/Logger.h"

#define NUM_CHANNELS 2
#define CHANNEL_RELIABLE 0
#define CHANNEL_UNRELIABLE 1

static std::string generateUniqueName() {
	static unsigned long long number = 0;
	return "player" + std::to_string(number++);
}

Connection::Connection() : mHost(nullptr),
	mServing(false),
	mConnected(false),
	mClientToServer(nullptr),
	mPlayerName(""),
	mPing(0) {

}

void Connection::processNetworkEvents() {
	if (!mHost) return;

	mServing ? processServer() : processClient();
}

void Connection::processClient() {
	ENetEvent evt;
	while (enet_host_service(mHost, &evt, 0)) {
		switch (evt.type) {
		case ENET_EVENT_TYPE_CONNECT:
			onConnectedToServer();
			break;

		case ENET_EVENT_TYPE_DISCONNECT:
			onServerDisconnected();
			break;

		case ENET_EVENT_TYPE_RECEIVE:
			BitStream stream(evt.packet->data, evt.packet->dataLength);
			onServerData(stream);
			enet_packet_destroy(evt.packet);
			break;
		}
	}
}

bool Connection::isFullyReady() const {
	return isServer() ? mConnected : mConnected && !mPlayerName.empty() && mPing > .00001f;
}

void Connection::onConnectedToServer() {
	mConnected = true;
	gLogger.info("Connected to server\n");
}

void Connection::onServerDisconnected() {
	mConnected = false;
	emit("serverdisconnect");
}

void Connection::onServerData(const BitStream &stream) {
	std::string event = stream.readString();
	if (event == "yourname")
		mPlayerName = stream.readString();
}

void Connection::processServer() {
	ENetEvent evt;
	while (enet_host_service(mHost, &evt, 0)) {
		switch (evt.type) {
		case ENET_EVENT_TYPE_CONNECT:
			onClientConnected(evt.peer);
			break;

		case ENET_EVENT_TYPE_DISCONNECT:
			onClientDisconnected(evt.peer);
			break;

		case ENET_EVENT_TYPE_RECEIVE:
			BitStream stream(evt.packet->data, evt.packet->dataLength);
			onClientData(stream, evt.peer);

			enet_packet_destroy(evt.packet);
			break;
		}
	}
}

void Connection::onClientData(const BitStream &stream, ENetPeer *peer) {
	gLogger.info("Received from client %s %s\n", stream.readString().c_str(), stream.readString().c_str());
}

void Connection::onClientConnected(ENetPeer *peer) {
	std::string name = generateUniqueName();

	mClientsToNames[peer] = name;
	mNamesToClients[name] = peer;

	emit<const std::string&>("playerentered", name);

	static uint8_t streamBuffer[512];
	BitStream stream(streamBuffer, sizeof(streamBuffer));
	stream.writeString("yourname");
	stream.writeString(name);

	send(stream, true, name);

	gLogger.info("client connected %s\n", name.c_str());
}

void Connection::onClientDisconnected(ENetPeer *peer) {
	if (mClientsToNames.find(peer) != mClientsToNames.end()) {
		auto &name = mClientsToNames[peer];
		mClientsToNames.erase(peer);
		mNamesToClients.erase(name);
	}
}

void Connection::connect(const std::string &hostName, unsigned short port) {
	enet_initialize();

	mHost = enet_host_create (NULL /* create a client host */,
				1 /* only allow 1 outgoing connection */,
				NUM_CHANNELS,
				0,
				0);

	assert(mHost != nullptr);
	mServing = false;

	ENetAddress finalAddr;
	enet_address_set_host(&finalAddr, hostName.c_str());
	finalAddr.port = port;

	mClientToServer = enet_host_connect(mHost, &finalAddr, 2, 0);
}

void Connection::serve(unsigned short port) {
	enet_initialize();
	ENetAddress address;

	/* Bind the server to the default localhost.     */
	/* A specific host address can be specified by   */
	/* enet_address_set_host (& address, "x.x.x.x"); */
	address.host = ENET_HOST_ANY;
	/* Bind the server to port 1234. */
	address.port = port;
	mHost = enet_host_create (&address /* the address to bind the server host to */,
								 32      /* allow up to 32 clients and/or outgoing connections */,
								  NUM_CHANNELS,
								  0      /* assume any amount of incoming bandwidth */,
								  0      /* assume any amount of outgoing bandwidth */);

	mServing = true;
	mConnected = true;
}

void Connection::send(const BitStream &stream, bool reliable, const std::string &receiver) {
	send(stream.getDataBuffer(), stream.getSize(), reliable, receiver);
}

void Connection::send(const uint8_t *data, int size, bool reliable, const std::string &receiver) {
	if (!mHost) return;

	ENetPacket *packet = enet_packet_create(data, size, reliable ? ENET_PACKET_FLAG_RELIABLE : 0);

	if (mServing && !receiver.empty() && mNamesToClients.find(receiver) != mNamesToClients.end()) {
		sendTo(packet, mNamesToClients[receiver]);
	}
	else if (mServing) {
		// Broadcast
		for (auto &kv : mNamesToClients) {
			sendTo(packet, kv.second);
		}
	}
	else if (!mServing && mClientToServer) {
		sendTo(packet, mClientToServer);
	}
}

void Connection::disconnect() {
	if (mHost) {
		enet_host_destroy(mHost);
		mHost = nullptr;
	}
}

void Connection::sendTo(ENetPacket *packet, ENetPeer *peer) {
	enet_peer_send(peer, (packet->flags & ENET_PACKET_FLAG_RELIABLE) ? CHANNEL_RELIABLE : CHANNEL_UNRELIABLE, packet);
}
