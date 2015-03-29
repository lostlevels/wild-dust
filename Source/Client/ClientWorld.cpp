#include "Precompiled.h"
#include "ClientWorld.h"
#include "Core/Logger.h"
#include "Input/Input.h"
#include "Audio/Audio.h"
#include "Audio/SoundEffect.h"
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
	mShotSound(nullptr),
	mDeathSound(nullptr),
	mHitSound(nullptr),
	mSnapshotTimer(0),
	mGibs(nullptr),
	mSendTimer(0),
	mTmxMap(nullptr)
{
	if (mAudio) {
		mShotSound = mAudio->createSoundEffect();
		mShotSound->loadWave("../Content/SFX/shot.wav");

		mDeathSound = mAudio->createSoundEffect();
		mDeathSound->loadWave("../Content/SFX/ech_death.wav");

		mHitSound = mAudio->createSoundEffect();
		mHitSound->loadWave("../Content/SFX/ech.wav");
	}

	mStream = new BitStream(16 * 1024);
	addDefaultMap();
	addGibs();
	addHearts();
}

void ClientWorld::addDefaultMap() {
	auto map = new TilemapEntity("01map", "../Content/Maps/Default.tmx");
	mTmxMap = &map->getMap();
	scheduleAddEntity(map);

	// Note 00stars is a hack to make stars drawn first, similar thing with 01map
	auto stars = new StarsEntity("00stars", {{8, 8}, "../Content/Textures/Misc/Star.png"});
	scheduleAddEntity(stars);
}

void ClientWorld::addGibs() {
	mGibs = new GibCollectionEntity(this, "gibs", {{8, 8}, "../Content/Textures/Misc/Gib.png"});
	scheduleAddEntity(mGibs);
}

void ClientWorld::setHeartsDisplay(float health) {
	auto numHearts = std::min((std::size_t)health, mHearts.size());
	for (std::size_t i = 0; i < numHearts; ++i)
		mHearts[i]->setActive(true);
	for (std::size_t i = numHearts; i < mHearts.size(); ++i)
		mHearts[i]->setActive(false);
}

void ClientWorld::addHearts() {
	if (!mHearts.empty()) return;

	#define MAX_VISIBLE_HEARTS 4
	mHearts.resize(MAX_VISIBLE_HEARTS);
	for (int i = 0; i < MAX_VISIBLE_HEARTS; ++i) {
		auto heart = new Entity("z" + createUniqueEntId(), "gui", "", SEND_NEVER, {{32, 32}, "../Content/Textures/Misc/Heart.png", {0, 0, 16, 16}});
		heart->setPosition(Vec3(16 + 36 * i, 8, 0));
		mHearts[i] = heart;
		scheduleAddEntity(heart);
	}
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

void ClientWorld::applyDamage(const std::string &entId, float amount) {
	auto me = getMe();
	auto ent = getEntity(entId);
	if (me && ent) {
		me->addSnapshot(CommandSnapshot(mConn.getServerTime(), "damage", me->getId(), entId));
	}
}

ClientWorld::~ClientWorld() {
	if (mAudio) {
		if (mHitSound) mAudio->destroySoundEffect(mHitSound);
		if (mDeathSound) mAudio->destroySoundEffect(mDeathSound);
		if (mShotSound) mAudio->destroySoundEffect(mShotSound);
		// We dont own audio or sound so dont delete them
	}

	delete mStream;
	mStream = nullptr;
}

void ClientWorld::fillGUIData(int screenWidth, int screenHeight, std::vector<GUIData> &data) {
	int startingY = 40;
	int y = startingY;
	int x = 20;
	int verticalSpacing = 20;
	char buffer[256];

	if (mInput && mInput->getButtons() & BTN_INFO) {
		snprintf(buffer, sizeof(buffer) - 1, "%10s%10s%10s%10s", "Name", "Kills", "Deaths", "Ping");
		data.push_back({x, y, buffer});
		y += verticalSpacing;

		for (auto &kv : mPlayerStates) {
			std::string name = kv.first;
			if (name == mConn.getName()) name = "you";
			auto &state = kv.second;
			snprintf(buffer, sizeof(buffer) - 1, "%10s%10d%10d%10d", name.c_str(), state.kills, state.deaths, (int)(state.ping * 1000));
			data.push_back({x, y, buffer});
			y += verticalSpacing;
		}
	}
	else {
		data.push_back({x, y, "Tab for stats"});
		y += verticalSpacing;
	}

	y = startingY;
	x = screenWidth - 350;
	for (auto &message : mGameMessages) {
		data.push_back({x, y, message});
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
	std::string myName = mConn.getName();
	float myPing = getPing(myName);
	for (auto &kv : mEntities) {
		auto ent = kv.second;
		bool lerp = ent->getOwner() != myName && ent->isRemote() && ent->getSendMode() == SEND_ALWAYS;
		if (lerp) {
			float ping = getPing(ent->getOwner());
			float viewDelay = std::max(.125f, ping + myPing + SEND_RATE * 2.1f);
			ent->setViewDelay(viewDelay);
		}
	}

	float timestep = 1 / 60.0f;
	// while (dt > 0.000001f) {
		float delta = dt >= timestep ? timestep : dt;
		delta = dt;
		handlePlayerInput(delta);
		sendQueuedPackets(delta);

		World::update(gameTime, delta);

		dt -= delta;
	// }

	mConn.processNetworkEvents();
}

void ClientWorld::spawnProjectile(const std::string &type, const std::string &owner, const Vec3 &position, float rotation) {
	// Handle informing server ...
	float speed = 2500;
	float theta = glm::radians(rotation);
	Vec3 velocity(cosf(theta) * speed, sinf(theta) * speed, 0);

	auto ownerEnt = getEntity(owner);
	bool local = ownerEnt ? ownerEnt->isLocal() : false;
	auto ent = local ? ProjectileFactory::createLocalProjectile(this, createUniqueEntId(), "bullet", owner, velocity) : 
		ProjectileFactory::createRemoteProjectile(this, createUniqueEntId(), "bullet", owner, velocity);
	ent->setPosition(position);
	ent->setFlip(rotation > .000001f ? 1 : 0);
	scheduleAddEntity(ent);

	// Add a command snapshot if it's us creating the bullet.
	// auto me = getMe();
	// if (me && me->getId() == owner) {
	// 	me->addSnapshot(CommandSnapshot(mConn.getServerTime(), "spawnprojectile", owner, "", position));
	// }

	if (mShotSound) mShotSound->play();
}

void ClientWorld::spawnGibs(const Vec3 &position, int amount) {
	if (mGibs) mGibs->spawnGibs(position, amount);
	if (mHitSound) mHitSound->play();
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

	mConn.onBitStream("you", [&](const BitStream &stream) {
		stream.rewind();
		stream.readString(); // Should be name of the event, "you"
		Entity *e = EntitySerializer::deserializeInitialEntity(this, stream);
		scheduleAddEntity(e->getId(), e);
		gLogger.info("Added you\n");
	});

	mConn.onBitStream("gamemessage", [&](const BitStream &stream) {
		stream.rewind();
		stream.readString();
		std::string message = stream.readString();
		auto namePos = message.find(mConn.getName());
		if (namePos != std::string::npos) {
			message.replace(namePos, mConn.getName().length(), "you");
		}
		addGameMessage(message);
	});

	mConn.onBitStream("death", [&](const BitStream &stream) {
		stream.rewind();
		stream.readString();
		std::string dyer = stream.readString();
		Vec3 newPosition;
		newPosition.x = stream.readFloat();
		newPosition.y = stream.readFloat();
		newPosition.z = stream.readFloat();
		auto ent = getEntity(dyer);
		if (ent) {
			ent->setPosition(newPosition);
			ent->removeAllTransformSnapshots(); // DONT remove command snapshots in case it is ourself
			if (ent != getMe()) {
				ent->addSnapshot({mConn.getServerTime(), newPosition, 0, 0});
			}
		}
	});

	mConn.onBitStream("gamestate", std::bind(&ClientWorld::onGameState, this, std::placeholders::_1));
	mConn.onBitStream("playerexit", std::bind(&ClientWorld::onPlayerExit, this, std::placeholders::_1));
	mConn.onBitStream("allplayersupdate", std::bind(&ClientWorld::onAllPlayersUpdate, this, std::placeholders::_1));
}

void ClientWorld::onPlayerExit(const BitStream &stream) {
	stream.rewind();
	stream.readString();
	std::string name = stream.readString();

	mPlayerStates.erase(name);
	scheduleDeleteEntity(name);

	addGameMessage("%s left", name.c_str());
}

void ClientWorld::addGameMessage(const char *format, ...) {
	va_list args;
	va_start(args, format);
	char message[1024] = {0};
	vsprintf(message, format, args);
	va_end(args);

	mGameMessages.push_back(message);
	trimGameMessages();
}

void ClientWorld::addGameMessage(const std::string &message) {
	mGameMessages.push_back(message);
	trimGameMessages();
}

void ClientWorld::trimGameMessages() {
	if (mGameMessages.size() > 10)
		mGameMessages.erase(mGameMessages.begin(), mGameMessages.begin() + mGameMessages.size() - 10);
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
			ent->removeOldSnapshots();
		}
	}
	emptyEntity.removeAllSnapshots();
}

PlayerState *ClientWorld::getPlayerState(const std::string &id) {
	if (mPlayerStates.find(id) == mPlayerStates.end()) return nullptr;
	return &mPlayerStates[id];
}

void ClientWorld::onGameState(const BitStream &stream) {
	stream.rewind();
	std::string evt = stream.readString(); // gamestate

	uint32_t players = stream.readU32();
	for (uint32_t i = 0; i < players; ++i) {
		PlayerState state = EntitySerializer::deserializePlayerState(stream);
		if (state.name != mConn.getName() && mEntities.find(state.name) == mEntities.end()) {
			scheduleAddEntity(state.name, EntityFactory::createRemoteEntity(this, state.name, state.type, state.name));
			// gLogger.info("Added other player '%s'\n", state.name.c_str());
			addGameMessage("%s entered", state.name.c_str());
		}
		mPlayerStates[state.name] = state;
		// gLogger.info("%f\n", state.ping);
	}
	
	auto state = getPlayerState(mConn.getName());
	if (!state) return;

	setHeartsDisplay(state->health);
}

Entity *ClientWorld::getMe() {
	auto &name = mConn.getName();
	return !name.empty() ? getEntity(name) : nullptr;
}
