#include "Precompiled.h"
#include "Connection.h"
#include "Core/Logger.h"
#include <string>

#define NUM_CHANNELS 2
#define CHANNEL_RELIABLE 0
#define CHANNEL_UNRELIABLE 1

#define MIN_PACKETS_NEEDED_FOR_PING 10

static std::string generateUniqueName() {
	static unsigned long long number = 0;
	return "player" + std::to_string(number++);
}

static uint8_t scrachBuffer[8096];

static BitStream getTempBistream() {
	return BitStream(scrachBuffer, sizeof(scrachBuffer));
}

#pragma mark -
#pragma mark Note

// Only non bitstream events emitted are
// Server emits
//    cliententered -> name
//    clientexit -> name
//    cliententeredandpinged -> name
// Client emits
//    serverdisconnect -> no args
// Otherwise event name that is sent is the emitted event along with a bitstream
// The bitstream's first value is always a string that is the name of the event again.
// Bitstream event handlers should call stream.rewind() first to clear any reads from previous handlers
// conn.on("customEvent", [](const BitStream &stream) {
//   stream.rewind();
//   ... do logic ...
// })
//

#pragma mark -
#pragma mark Connection

Connection::Connection() :
	mHost(nullptr),
	mClientToServer(nullptr),
	mServing(false),
	mConnected(false),
	mPlayerName(""),
	mPing(0),
	mServerTimeOffset(0) {

}

void Connection::processNetworkEvents() {
	if (!mHost) return;

	mServing ? processServer() : processClient();
}

void Connection::processClient() {
	ENetEvent evt;
	while (enet_host_service(mHost, &evt, 0)) {
		switch (evt.type) {
		case ENET_EVENT_TYPE_NONE: break;
		case ENET_EVENT_TYPE_CONNECT:
			onConnectedToServer();
			break;

		case ENET_EVENT_TYPE_DISCONNECT:
			onServerDisconnected();
			break;

		case ENET_EVENT_TYPE_RECEIVE:
			BitStream stream(evt.packet->data, evt.packet->dataLength);
			std::string event = stream.readString();
			stream.rewind();
			onServerData(stream);
			emitStream(event, stream);

			enet_packet_destroy(evt.packet);
			break;
		}
	}
}

float Connection::getClientPing(const std::string &name) const {
	auto kv = mNamesToClients.find(name);
	return kv != mNamesToClients.end() ? kv->second->mPing : 0;
}

bool Connection::isFullyReady() const {
	return isServer() ? mConnected : mConnected && !mPlayerName.empty() && mPing > .00001f;
}

float Connection::getServerTime() const {
	return isServer() ? mClock.getElapsedSeconds() : mClock.getElapsedSeconds() + mServerTimeOffset;
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
	else if (event == "ping")
		onPinged(stream);
	else if (event == "latency") {
		float serverTime = stream.readFloat();
		mPing = stream.readFloat();

		mServerTimeOffset = serverTime + mPing/2.0f - mClock.getElapsedSeconds();
	}
}

void Connection::processServer() {
	ENetEvent evt;
	while (enet_host_service(mHost, &evt, 0)) {
		switch (evt.type) {
		case ENET_EVENT_TYPE_NONE: break;
		case ENET_EVENT_TYPE_CONNECT:
			onClientConnected(evt.peer);
			break;

		case ENET_EVENT_TYPE_DISCONNECT:
			onClientDisconnected(evt.peer);
			break;

		case ENET_EVENT_TYPE_RECEIVE:
			BitStream stream(evt.packet->data, evt.packet->dataLength);

			std::string event = stream.readString();
			stream.rewind();
			onClientData(stream, evt.peer);
			emitStream(event, stream);

			enet_packet_destroy(evt.packet);
			break;
		}
	}

	updatePings();
}

void Connection::emitStream(const std::string &event, const BitStream &stream) {
	emit<const BitStream&>(event, stream);
}

void Connection::updatePings() {
	for (auto &kv : mPeerToClients) {
		auto client = kv.second;

		// Ping every .10 seconds.
		if (client->mPingClock.getElapsedSeconds() > .10f && client->mPings.size() < MIN_PACKETS_NEEDED_FOR_PING) {
			client->mPingClock.reset();
			pingClient(client->mName);
		}
	}
}

void Connection::onClientData(const BitStream &stream, ENetPeer *peer) {
	if (mPeerToClients.find(peer) == mPeerToClients.end()) return;

	auto client = mPeerToClients[peer];
	auto event = stream.readString();

	if (event == "pong") {
		float time = mClock.getElapsedSeconds() - stream.readFloat();
		if (time < .00000001f)
			return;

		client->mPings.push_back(time);

		if (client->mPings.size() >= MIN_PACKETS_NEEDED_FOR_PING) {
			std::sort(client->mPings.begin(), client->mPings.end());
			client->mPing = client->mPings[client->mPings.size() / 2];
			// gLogger.info("Client ping median is %f\n", client->mPing);

			BitStream latencyStream = getTempBistream();
			writeStream<std::string, float, float>(latencyStream, "latency", mClock.getElapsedSeconds(), client->mPing);

			// Might have to be careful of reliable here in case it messes with time
			send(latencyStream, true, client->mName);

			emit<const std::string&>("cliententeredandpinged", client->mName);
		}
	}
}

void Connection::onClientConnected(ENetPeer *peer) {
	std::string name = generateUniqueName();

	auto clientData = new ClientConnectionData(name, peer);

	mPeerToClients[peer] = clientData;
	mNamesToClients[name] = clientData;

	emit<const std::string&>("cliententered", name);

	BitStream stream = getTempBistream();
	stream.writeString("yourname");
	stream.writeString(name);

	send(stream, true, name);
	pingClient(name);

	// gLogger.info("client connected %s\n", name.c_str());
}

void Connection::onClientDisconnected(ENetPeer *peer) {
	if (mPeerToClients.find(peer) != mPeerToClients.end()) {
		std::string name = mPeerToClients[peer]->mName;

		delete mPeerToClients[peer];

		mPeerToClients.erase(peer);
		mNamesToClients.erase(name);

		emit<const std::string&>("clientexit", name);
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

void Connection::pingClient(const std::string &client) {
	BitStream stream = getTempBistream();
	writeStream<std::string, float>(stream, "ping", mClock.getElapsedSeconds());
	send(stream, false, client);
}

void Connection::onPinged(const BitStream &stream) {
	// Pong it back, need to reconstruct since stream's size changes on read and write.
	BitStream packet = getTempBistream();
	writeStream<std::string, float>(packet, "pong", stream.readFloat());

	send(packet, false);
}

void Connection::send(const BitStream &stream, bool reliable, const std::string &receiver) {
	send(stream.getDataBuffer(), stream.getSize(), reliable, receiver);
}

void Connection::send(const uint8_t *data, int size, bool reliable, const std::string &receiver) {
	if (!mHost) return;
	assert(size < sizeof(scrachBuffer));

	ENetPacket *packet = enet_packet_create(data, size, reliable ? ENET_PACKET_FLAG_RELIABLE : 0);

	if (mServing && !receiver.empty() && mNamesToClients.find(receiver) != mNamesToClients.end()) {
		sendTo(packet, mNamesToClients[receiver]->mPeer);
	}
	else if (mServing) {
		// Broadcast
		for (auto &kv : mNamesToClients) {
			sendTo(packet, kv.second->mPeer);
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
