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
#define STARTING_HEALTH 4

ServerWorld::ServerWorld() :
	mStateSendTimer(0),
	mLastCharacter(""),
	mGameStateSendTimer(0) {
	mStream = new BitStream(16 * 1024);
	mEntsToSend.reserve(256);
}

ServerWorld::~ServerWorld() {
	delete mStream;
	mStream = nullptr;
}

int ServerWorld::getTickRate() const {
	return 60;
}

int ServerWorld::getSendRate() const {
	return (int)(1.0f / SEND_RATE);
}

void ServerWorld::update(float dt) {
	update(mConn.getServerTime(), dt);
}

void ServerWorld::update(float gameTime, float dt) {

	float timestep = 1 / 60.0f;
	//while (dt > 0.000001f) {
		float delta = dt >= timestep ? timestep : dt;
		delta = dt;
		handleStateUpdates(delta);
		handleSendGameStates(delta);
		World::update(gameTime, delta);

		dt -= delta;
	// }
	mConn.processNetworkEvents();
}

void ServerWorld::handleSendGameStates(float dt) {
	mGameStateSendTimer += dt;
	if (mGameStateSendTimer > 1.0f) {
		mGameStateSendTimer -= 1.0f;
		for (auto &kv : mPlayerStates) {
			kv.second.ping = mConn.getClientPing(kv.first);
		}
		sendGameState();
	}
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

void ServerWorld::serve(unsigned short port) {
	mPort = port;
	mConn.serve(port);

	auto clientEntered = std::bind(&ServerWorld::onClientEntered, this, std::placeholders::_1);
	// mConn.on("cliententered", clientEntered);
	mConn.onString("cliententeredandpinged", clientEntered);

	// Getting lazy so just put the handler as a lambda
	mConn.onString("clientexit", [&](const std::string &playerName) {
		if (mPlayerStates.find(playerName) == mPlayerStates.end()) return;

		mPlayerStates.erase(playerName);

		for (auto &kv : mEntities) {
			if (kv.second->getOwner() == playerName) {
				scheduleDeleteEntity(kv.second);
			}
		}

		sendPlayerLeft(playerName);
		gLogger.info("Player left: %s\n", playerName.c_str());
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

	std::vector<CommandSnapshot> commands;
	e->getCommandSnapshotsAfter(latestCommandTime, commands);
	for (auto &snapshot : commands) {
		processCommand(snapshot);
	}
}

void ServerWorld::processCommand(const CommandSnapshot &snapshot) {
	if (snapshot.command == "damage") {
		auto receiverState = getPlayerState(snapshot.receiver);
		auto shooterState = getPlayerState(snapshot.owner);

		if (!receiverState || !shooterState) return;
		receiverState->health -= 1.0f;
		if (receiverState->health < .0001f) {
			receiverState->health = STARTING_HEALTH;
			receiverState->deaths++;
			shooterState->kills++;
			sendDeathMessage(snapshot.receiver);
			sendGameMessage("%s killed %s", snapshot.owner.c_str(), snapshot.receiver.c_str());
		}
		// Force send next update
		mGameStateSendTimer = 1.0f;
	}
}

void ServerWorld::sendPlayerLeft(const std::string &id) {
	mStream->rewind();
	mStream->writeString("playerexit");
	mStream->writeString(id);
	mConn.send(*mStream, true);
}

void ServerWorld::sendGameMessage(const char *format, ...) {
	va_list args;
	va_start(args, format);
	char message[1024] = {0};
	vsprintf(message, format, args);
	va_end(args);

	mStream->rewind();
	mStream->writeString("gamemessage");
	mStream->writeString(message);

	mConn.send(*mStream, true);
}

void ServerWorld::sendDeathMessage(const std::string &dyingEntId) {
	mStream->rewind();
	mStream->writeString("death");
	mStream->writeString(dyingEntId);
	Vec3 newPosition(1280/2, -15, 0);
	mStream->writeFloat(newPosition.x);
	mStream->writeFloat(newPosition.y);
	mStream->writeFloat(newPosition.z);
	mConn.send(*mStream, true);
}

PlayerState *ServerWorld::getPlayerState(const std::string &id) {
	if (mPlayerStates.find(id) == mPlayerStates.end()) return nullptr;
	return &mPlayerStates[id];
}

void ServerWorld::onClientEntered(const std::string &playerName) {
	// Add to list of players.
	// Notify all (existing and new player) of new player.
	// Send list of players with state to player.
	gLogger.info("Player entered: %s\n", playerName.c_str());

	std::vector<std::string> types = {"cowboy", "bandit", "doc", "barista", "cop"};
	auto find = std::find(types.begin(), types.end(), mLastCharacter);
	if (find == types.end()) find = types.begin();
	else if (++find == types.end()) find = types.begin();
	std::string type = (*find);
	mLastCharacter = (*find);

	PlayerState playerState;
	playerState.name = playerName;
	playerState.type = type;
	playerState.health = STARTING_HEALTH;
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
