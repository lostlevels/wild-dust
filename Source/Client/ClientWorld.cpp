#include "Precompiled.h"
#include "ClientWorld.h"
#include "Core/Logger.h"
#include "Input/Input.h"
#include "Audio/Audio.h"
#include "Serialization/EntitySerializer.h"
#include "Core/Entity.h"
#include "Core/EntityFactory.h"

#include <functional>

ClientWorld::ClientWorld(InputSystem *input, AudioSystem *audioSystem) :
	mInput(input),
	mAudio(audioSystem),
	mSnapshotTimer(0),
	mSendTimer(0)
{
	mStream = new BitStream(16 * 1024);
}

ClientWorld::~ClientWorld() {
	delete mStream;
	mStream = nullptr;
}

void ClientWorld::update(float dt) {
	World::update(dt);
	mConn.processNetworkEvents();
	// Handle game logic ...
	handlePlayerInput(dt);
	sendQueuedPackets(dt);

	std::string myName = mConn.getName();
	float myPing = getPing(myName);
	for (auto &kv : mEntities) {
		auto ent = kv.second;
		bool lerp = ent->getOwner() != myName && ent->getSendMode() == SEND_ALWAYS;
		if (lerp) {
			float ping = getPing(ent->getOwner());
			float viewDelay = std::max(.1f, ping/2 + myPing/2 + .05f*2 + .05f);
			ent->interpolate(mConn.getServerTime() - viewDelay);
		}
	}
}

void ClientWorld::sendQueuedPackets(float dt) {
	auto me = getMe();
	if (!me) return;

	if ((mSendTimer += dt) > .05f) {
		// Send then remove.
		mSendTimer -= .05f;

		mStream->rewind();
		mStream->writeString("playerupdate");
		mStream->writeString(mConn.getName());
		EntitySerializer::serializeExistingEntity(*me, *mStream);
		mConn.send(*mStream, false);

		me->removeAllSnapshots();
	}
}

float ClientWorld::getPing(const std::string &name) const {
	const auto &kv = mPlayerStates.find(name);
	return kv != mPlayerStates.end() ? kv->second.ping : 0;
}

void ClientWorld::handlePlayerInput(float dt) {
	auto me = getMe();
	if (!me || !mInput) return;

	auto position = me->getPosition();
	if (mInput->getButtons() & BTN_MOVE_LEFT) {
		position.x -= 500 * dt;
	}
	if (mInput->getButtons() & BTN_MOVE_RIGHT) {
		position.x += 500 * dt;
	}

	me->setPosition(position);
	if ((mSnapshotTimer += dt) >= 1/60.0f) {
		mSnapshotTimer -= 1/60.0f;
		me->queueSnapshot(mConn.getServerTime());
	}
}

void ClientWorld::connect(const std::string &hostName, unsigned short port) {
	mConn.connect(hostName, port);

	mConn.on("you", [&](const BitStream &stream) {
		stream.rewind();
		stream.readString(); // Should be name of the event, "you"
		Entity *e = EntitySerializer::deserializeInitialEntity(stream);
		addEntity(e->getId(), e);
		gLogger.info("Added you\n");
	});

	mConn.on("gamestate", [&](const BitStream &stream) {
		stream.rewind();
		std::string evt = stream.readString(); // gamestate
		assert(evt == "gamestate");

		uint32_t players = stream.readU32();
		for (uint32_t i = 0; i < players; ++i) {
			PlayerState state = EntitySerializer::deserializePlayerState(stream);
			if (state.name != mConn.getName() && mEntities.find(state.name) == mEntities.end()) {
				addEntity(state.name, EntityFactory::createEntity(state.name, state.type, state.name));
				gLogger.info("Added other '%s' '%s'\n", state.name.c_str(), mConn.getName().c_str());
			}
			mPlayerStates[state.name] = state;
			gLogger.info("%f\n", state.ping);
		}
	});

	mConn.on("playerexit", [&](const BitStream &stream) {
		stream.rewind();
		stream.readString();
		std::string name = stream.readString();

		// Remove player name .... todo:
	});

	mConn.on("allplayersupdate", [&](const BitStream &stream) {
		stream.rewind();
		stream.readString(); // Should be name of the event, "allplayersupdate"

		static Entity emptyEntity;
		uint32_t numPlayers = stream.readU32();
		// Kind of sucks, but deserializing here to handle some special logic.
		for (uint32_t i = 0; i < numPlayers; ++i) {
			std::string name = stream.readString();
			auto ent = getEntity(name);
			ent = ent && name != mConn.getName() ? ent : &emptyEntity;
			EntitySerializer::deserializeIntoEntity(ent, stream);
		}
		emptyEntity.removeAllSnapshots();
	});
}

Entity *ClientWorld::getMe() {
	auto &name = mConn.getName();
	return !name.empty() ? getEntity(name) : nullptr;
}
