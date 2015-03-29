#pragma once

#include "Core/World.h"
#include "Core/BitStream.h"
#include "Core/Clock.h"
#include "Core/PlayerState.h"
#include "Core/WorldCollision.h"
#include "Network/Connection.h"
#include "Game/ProjectileSpawner.h"
#include "Game/GameContext.h"
#include "Input/Input.h"
#include "Core/GibCollectionEntity.h"
#include "Game/GibSpawner.h"
#include <Tmx.h>

class AudioSystem;

class ClientWorld : public World, public ProjectileSpawner, public GameContext, public WorldCollision, public GibSpawner {
public:
	ClientWorld(InputSystem *input, AudioSystem *audioSystem);
	virtual ~ClientWorld();

	void connect(const std::string &hostName, unsigned short port);
	void update(float dt);

	virtual void spawnProjectile(const std::string &type, const std::string &owner, const Vec3 &position, float rotation);

	// GameContext interface methods
	virtual InputBase *getInput() { return mInput; }
	virtual ProjectileSpawner *getProjectileSpawner() { return this; }
	virtual World *getWorld() { return this; }

	virtual WorldCollision *getCollision() { return this; }
	virtual bool collides(float x, float y) const;

	virtual GibSpawner *getGibSpawner() { return this; }
	virtual void spawnGibs(const Vec3 &position, int amount) { if (mGibs) mGibs->spawnGibs(position, amount); }

private:
	GibCollectionEntity *mGibs;
	std::map<std::string, PlayerState> mPlayerStates;

	Connection     mConn;
	BitStream      *mStream;

	InputSystem    *mInput;
	AudioSystem    *mAudio;

	float           mSnapshotTimer;
	float           mSendTimer;

	Tmx::Map        *mTmxMap;

	Entity *getMe();
	void handlePlayerInput(float dt);
	void sendQueuedPackets(float dt);

	std::string    createUniqueEntId() const;

	float getPing(const std::string &name) const;

	void onGameState(const BitStream &stream);
	void onPlayerExit(const BitStream &stream);
	void onAllPlayersUpdate(const BitStream &stream);

	void addDefaultMap();
	void addGibs();

	// Make derived update method private so we can use serverTime as gameTime
	virtual void update(float gameTime, float dt);
};
