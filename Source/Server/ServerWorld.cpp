#include "Precompiled.h"
#include "ServerWorld.h"
#include "Core/EntityFactory.h"
#include "Core/Entity.h"
#include "Core/Logger.h"
#include "Serialization/EntitySerializer.h"
#include <functional>
#include "Core/PlayerState.h"


ServerWorld::ServerWorld() :
	mStateSendTimer(0) {
	mStream = new BitStream(16 * 1024);
	mEntsToSend.reserve(256);
}

ServerWorld::~ServerWorld() {
	delete mStream;
	mStream = nullptr;
}

void ServerWorld::update(float dt) {
	World::update(dt);
	mConn.processNetworkEvents();
	handleStateUpdates(dt);
}

void ServerWorld::sendGameState() {
	// Send reliabily every score update / player enter

	mStream->rewind();
	mStream->writeString("gamestate");
	mStream->writeU32(mPlayerStates.size());
	for (auto &kv : mPlayerStates) {
		EntitySerializer::serializePlayerState(kv.second, *mStream);
	}

	mConn.send(*mStream, true);
}

void ServerWorld::handleStateUpdates(float dt) {
	mStateSendTimer += dt;
	if (mStateSendTimer > .05f) {
		mStateSendTimer -= .05f;

		mEntsToSend.clear();
		for (auto &kv : mEntities) {
			if (kv.second->getSendMode() == SEND_ALWAYS) {
				mEntsToSend.push_back(kv.second);
			}
		}

		mStream->rewind();
		mStream->writeString("allplayersupdate");
		EntitySerializer::serializeEntities(mEntsToSend, *mStream);

		// Clear their snapshots for next history
		for (auto ent : mEntsToSend) {
			ent->removeAllSnapshots();
		}

		mConn.send(*mStream, false);
	}
}

void ServerWorld::serve(unsigned short ip) {
	mConn.serve(ip);

	auto clientEntered = std::bind(&ServerWorld::onClientEntered, this, std::placeholders::_1);
	// mConn.on("cliententered", clientEntered);
	mConn.on("cliententeredandpinged", clientEntered);

	// Getting lazy so just put the handler as a lambda
	mConn.on("clientexit", [&](const std::string &playerName) {
		if (mPlayerStates.find(playerName) == mPlayerStates.end()) return;

		mPlayerStates.erase(playerName);

		for (auto &kv : mEntities) {
			if (kv.second->getOwner() == playerName) {
				// schedule a delete.
			}
		}

		mStream->rewind();
		mStream->writeString("playerexit");
		mStream->writeString(playerName);
	});

	mConn.on("playerupdate", [&](const BitStream &stream) {
		stream.rewind();
		stream.readString();

		std::string player = stream.readString();
		Entity *e = getEntity(player);
		if (!e) return;

		EntitySerializer::deserializeIntoEntity(e, stream);
	});
}

void ServerWorld::onClientEntered(const std::string &playerName) {
	// Add to list of players.
	// Notify all (existing and new player) of new player.
	// Send list of players with state to player.
	gLogger.info("onPlayerEnter ::Player entered :: %s\n", playerName.c_str());

	std::string type = "cowboy";
	PlayerState playerState;
	playerState.name = playerName;
	playerState.type = type;
	playerState.ping = mConn.getClientPing(playerName);
	mPlayerStates[playerName] = playerState;
	gLogger.info("Ping sent is %f\n", playerState.ping);

	Entity *e = EntityFactory::createEntity(playerName, type, playerName, SEND_ALWAYS);
	Vec3 startingPosition(100, 100, 0);
	e->setPosition(startingPosition);
	addEntity(playerName, e);

	mStream->rewind();
	mStream->writeString("you");
	EntitySerializer::serializeInitialEntity(mConn.getServerTime(), *e, *mStream);
	mConn.send(*mStream, true, playerName);

	sendGameState();
}
