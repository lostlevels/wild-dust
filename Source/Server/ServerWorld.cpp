#include "Precompiled.h"
#include "ServerWorld.h"
#include "Game/EntityFactory.h"
#include "Core/Entity.h"
#include "Core/Logger.h"
#include "Serialization/EntitySerializer.h"
#include <functional>
#include "Core/PlayerState.h"

// TODO: Read from config
#define SEND_RATE .04f

ServerWorld::ServerWorld() :
	mStateSendTimer(0),
	mLastCharacter("") {
	mStream = new BitStream(16 * 1024);
	mEntsToSend.reserve(256);
}

ServerWorld::~ServerWorld() {
	delete mStream;
	mStream = nullptr;
}

void ServerWorld::update(float dt) {
	update(mConn.getServerTime(), dt);
}

void ServerWorld::update(float gameTime, float dt) {
	handleStateUpdates(dt);

	World::update(gameTime, dt);
	mConn.processNetworkEvents();
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
	if (mStateSendTimer > SEND_RATE) {
		mStateSendTimer -= SEND_RATE;

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
	mConn.onString("cliententeredandpinged", clientEntered);

	// Getting lazy so just put the handler as a lambda
	mConn.onString("clientexit", [&](const std::string &playerName) {
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

	mConn.onBitStream("playerupdate", std::bind(&ServerWorld::onPlayerUpdate, this, std::placeholders::_1));
}

void ServerWorld::onPlayerUpdate(const BitStream &stream) {
	stream.rewind();
	stream.readString();

	std::string player = stream.readString();
	Entity *e = getEntity(player);
	if (!e) return;

	// Get time before adding new commands
	float latestCommandTime = e->getLatestCommandTime();
	EntitySerializer::deserializeIntoEntity(e, stream);

	// TODO, only handle new commands since playerupdate may be called multiple times before clearing

	std::vector<CommandSnapshot> commands;
	e->getCommandSnapshotsAfter(latestCommandTime, commands);
	for (auto &snapshot : commands) {
		processCommand(snapshot);
	}
}

void ServerWorld::processCommand(const CommandSnapshot &snapshot) {

}

void ServerWorld::onClientEntered(const std::string &playerName) {
	// Add to list of players.
	// Notify all (existing and new player) of new player.
	// Send list of players with state to player.
	gLogger.info("Player entered: %s\n", playerName.c_str());

	std::string type = mLastCharacter == "cowboy" ? "bandit" : "cowboy";
	mLastCharacter = type;

	PlayerState playerState;
	playerState.name = playerName;
	playerState.type = type;
	playerState.ping = mConn.getClientPing(playerName);
	mPlayerStates[playerName] = playerState;

	// No context needed on server side since that's mostly visual / audio type stuff
	Entity *e = EntityFactory::createRemoteEntity(nullptr, playerName, type, playerName);
	// No entities on server are local.
	e->setLocal(false);
	Vec3 startingPosition(100, 100, 0);
	e->setPosition(startingPosition);
	scheduleAddEntity(playerName, e);

	mStream->rewind();
	mStream->writeString("you");
	EntitySerializer::serializeInitialEntity(mConn.getServerTime(), *e, *mStream);
	mConn.send(*mStream, true, playerName);

	sendGameState();
}
