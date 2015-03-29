#include "Precompiled.h"
#include "ClientWorld.h"
#include "Core/Logger.h"
#include "Input/Input.h"
#include "Audio/Audio.h"
#include "Core/Entity.h"
#include "Core/TilemapEntity.h"
#include "Game/EntityFactory.h"
#include "Game/ProjectileFactory.h"
#include "Game/PlayerController.h"
#include "Game/ProjectileController.h"
#include "Serialization/EntitySerializer.h"
#include "Core/CommandSnapshot.h"
#include "Core/GibCollectionEntity.h"
#include "Core/StarsEntity.h"
#include "Renderer/Renderer.h"
#include "Renderer/GUI.h"
#include <functional>

// TODO: Read from config
#define SEND_RATE .04f
#define TICK_RATE (1/60.0f)

ClientWorld::ClientWorld(InputSystem *input, AudioSystem *audioSystem) :
	mInput(input),
	mAudio(audioSystem),
	mSnapshotTimer(0),
	mGibs(nullptr),
	mSendTimer(0),
	mTmxMap(nullptr)
{
	mStream = new BitStream(16 * 1024);
	addDefaultMap();
	addGibs();
}

void ClientWorld::addDefaultMap() {
	auto map = new TilemapEntity("map", "../Content/Maps/Default.tmx");
	mTmxMap = &map->getMap();
	scheduleAddEntity(map);

	auto stars = new StarsEntity("stars", {{8, 8}, "../Content/Textures/Misc/Star.png"});
	scheduleAddEntity(stars);
}

void ClientWorld::addGibs() {
	mGibs = new GibCollectionEntity(this, "gibs", {{8, 8}, "../Content/Textures/Misc/Gib.png"});
	scheduleAddEntity(mGibs);
}

bool ClientWorld::collides(float x, float y) const {
	if (!mTmxMap) return false;
	int width = mTmxMap->GetWidth();
	int height = mTmxMap->GetHeight();
	int tileWidth = mTmxMap->GetTileWidth();
	int tileHeight = mTmxMap->GetTileHeight();

	if (!width || !height || !tileWidth || !tileHeight || !mTmxMap->GetNumTileLayers())
		return false;

	auto layer = mTmxMap->GetTileLayer(0);
	int tileX = (int)(floorf(x / tileWidth));
	int tileY = (int)(floorf(y / tileHeight));

	if (tileX < 0 || tileX >= width || tileY < 0 || tileY >= height) return false;

	return layer->GetTileTilesetIndex(tileX, tileY) != -1 && layer->GetTileId(tileX, tileY) != -1;
}

ClientWorld::~ClientWorld() {
	delete mStream;
	mStream = nullptr;
}

void ClientWorld::fillGUIData(int screenWidth, int screenHeight, std::vector<GUIData> &data) {
	int y = 20;
	int x = 20;
	int verticalSpacing = 20;
	char buffer[256];

	if (mInput && mInput->getButtons() & BTN_INFO) {
		snprintf(buffer, sizeof(buffer) - 1, "%10s%10s%10s", "Name:", "Kills:", "Deaths:");
		data.push_back({x, y, buffer});
		y += verticalSpacing;

		for (auto &kv : mPlayerStates) {
			std::string name = kv.first;
			if (name == mConn.getName()) name = "you";

			snprintf(buffer, sizeof(buffer) - 1, "%10s%10d", name.c_str(), kv.second.kills);
			data.push_back({x, y, buffer});
			y += verticalSpacing;
		}
	}
	else {
		data.push_back({x, y, "Tab for stats"});
		y += verticalSpacing;
	}
}

void ClientWorld::update(float dt) {
	update(mConn.getServerTime(), dt);
}

std::string ClientWorld::createUniqueEntId() const {
	static unsigned long long number = 0;
	return mConn.getName() + "_" + std::to_string(number++);
}

void ClientWorld::update(float gameTime, float dt) {
	// Handle game logic ...
	handlePlayerInput(dt);
	sendQueuedPackets(dt);

	World::update(gameTime, dt);
	mConn.processNetworkEvents();

	std::string myName = mConn.getName();
	float myPing = getPing(myName);
	for (auto &kv : mEntities) {
		auto ent = kv.second;
		bool lerp = ent->getOwner() != myName && ent->isRemote() && ent->getSendMode() == SEND_ALWAYS;
		if (lerp) {
			float ping = getPing(ent->getOwner());
			float viewDelay = std::max(.125f, ping/2 + myPing/2 + .05f*2 + .05f);
			ent->setViewDelay(viewDelay);
		}
	}
}

void ClientWorld::spawnProjectile(const std::string &type, const std::string &owner, const Vec3 &position, float rotation) {
	// Handle informing server ...
	float speed = 2500;
	float theta = glm::radians(rotation);
	Vec3 velocity(cosf(theta) * speed, sinf(theta) * speed, 0);

	auto ent = ProjectileFactory::createLocalProjectile(this, createUniqueEntId(), "bullet", owner, velocity);
	ent->setPosition(position);
	ent->setFlip(rotation > .000001f ? 1 : 0);
	scheduleAddEntity(ent);

	// Add a command snapshot if it's us creating the bullet.
	auto me = getMe();
	if (me && me->getId() == owner) {
		me->addSnapshot(CommandSnapshot(mConn.getServerTime(), "spawnprojectile", owner, position));
	}
}

void ClientWorld::sendQueuedPackets(float dt) {
	auto me = getMe();
	if (!me) return;

	if ((mSendTimer += dt) > SEND_RATE) {
		mSendTimer -= SEND_RATE;

		mStream->rewind();
		mStream->writeString("playerupdate");
		mStream->writeString(mConn.getName());
		EntitySerializer::serializeExistingEntity(*me, *mStream);
		mConn.send(*mStream, false);

		// Local entity, dont need to keep snapshots around other than
		// for informing server so remove snapshots after sending
		me->removeAllTransformSnapshots();
		me->removeAllCommandSnapshots();
	}
}

float ClientWorld::getPing(const std::string &name) const {
	const auto &kv = mPlayerStates.find(name);
	return kv != mPlayerStates.end() ? kv->second.ping : 0;
}

void ClientWorld::handlePlayerInput(float dt) {
	auto me = getMe();
	if (!me || !mInput) return;

	if ((mSnapshotTimer += dt) >= TICK_RATE) {
		mSnapshotTimer -= TICK_RATE;
		me->queueTransformSnapshot(mConn.getServerTime(), mInput->getButtons());
	}
}

void ClientWorld::connect(const std::string &hostName, unsigned short port) {
	mConn.connect(hostName, port);

	mConn.on("you", [&](const BitStream &stream) {
		stream.rewind();
		stream.readString(); // Should be name of the event, "you"
		Entity *e = EntitySerializer::deserializeInitialEntity(this, stream);
		scheduleAddEntity(e->getId(), e);
		gLogger.info("Added you\n");
	});

	mConn.on("gamestate", std::bind(&ClientWorld::onGameState, this, std::placeholders::_1));
	mConn.on("playerexit", std::bind(&ClientWorld::onPlayerExit, this, std::placeholders::_1));
	mConn.on("allplayersupdate", std::bind(&ClientWorld::onAllPlayersUpdate, this, std::placeholders::_1));
}

void ClientWorld::onPlayerExit(const BitStream &stream) {
	stream.rewind();
	stream.readString();
	std::string name = stream.readString();

	scheduleDeleteEntity(name);
}

void ClientWorld::onAllPlayersUpdate(const BitStream &stream) {
	stream.rewind();
	stream.readString(); // "allplayersupdate"

	static Entity emptyEntity;
	uint32_t numPlayers = stream.readU32();
	// Kind of sucks, but deserializing here to handle some special logic.
	for (uint32_t i = 0; i < numPlayers; ++i) {
		std::string name = stream.readString();
		auto ent = getEntity(name);
		bool isMe = name == mConn.getName();
		ent = ent && !isMe ? ent : &emptyEntity;
		EntitySerializer::deserializeIntoEntity(ent, stream);
		if (!isMe) {
			// Process and remove all commands now - hmm will just do in controller maybe ...
			// auto &snapshots = ent->getCommandSnapshots();
			// for (auto &snapshot : snapshots) {
			// 	if (snapshot.command == "spawnprojectile") {
			// 		float speed = 100;
			// 		float theta = glm::radians(0.0f);
			// 		Vec3 velocity(cosf(theta) * speed, sinf(theta) * speed, 0);
			// 		auto projectile = scheduleAddEntity(ProjectileFactory::createRemoteProjectile(this, createUniqueEntId(), "bullet", snapshot.owner, velocity));
			// 		projectile->setPosition(snapshot.position);
			// 	}
			// }
			ent->removeAllCommandSnapshots();
		}
	}
	emptyEntity.removeAllSnapshots();
}

void ClientWorld::onGameState(const BitStream &stream) {
	stream.rewind();
	std::string evt = stream.readString(); // gamestate

	uint32_t players = stream.readU32();
	for (uint32_t i = 0; i < players; ++i) {
		PlayerState state = EntitySerializer::deserializePlayerState(stream);
		if (state.name != mConn.getName() && mEntities.find(state.name) == mEntities.end()) {
			scheduleAddEntity(state.name, EntityFactory::createRemoteEntity(this, state.name, state.type, state.name));
			gLogger.info("Added other player '%s'\n", state.name.c_str());
		}
		mPlayerStates[state.name] = state;
		gLogger.info("%f\n", state.ping);
	}
}

Entity *ClientWorld::getMe() {
	auto &name = mConn.getName();
	return !name.empty() ? getEntity(name) : nullptr;
}
