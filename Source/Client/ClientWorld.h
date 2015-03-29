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
#include "Game/Damager.h"
#include "Renderer/GUIData.h"
#include <Tmx.h>

class AudioSystem;
class SoundEffect;

class CLIENT_API ClientWorld : public World, public ProjectileSpawner, public GameContext, public WorldCollision, public GibSpawner, public Damager {
public:
	ClientWorld(InputSystem *input, AudioSystem *audioSystem);
	virtual ~ClientWorld();

	void connect(const std::string &hostName, unsigned short port);
	void disconnect() { mConn.disconnect(); }
	void update(float dt);

	virtual void spawnProjectile(const std::string &type, const std::string &owner, const Vec3 &position, float rotation);

	// GameContext interface methods
	virtual InputBase *getInput() { return mInput; }
	virtual ProjectileSpawner *getProjectileSpawner() { return this; }
	virtual World *getWorld() { return this; }

	virtual WorldCollision *getCollision() { return this; }
	virtual bool collides(float x, float y) const;

	virtual GibSpawner *getGibSpawner() { return this; }
	virtual void spawnGibs(const Vec3 &position, int amount);

	virtual Damager *getDamager() { return this; }
	virtual void applyDamage(const std::string &entId, float amount);

	void fillGUIData(int screenWidth, int screenHeight, std::vector<GUIData> &data);

private:
	GibCollectionEntity *mGibs;
	std::map<std::string, PlayerState> mPlayerStates;

	std::vector<Entity*> mHearts;

	Connection     mConn;
	BitStream      *mStream;

	InputSystem    *mInput;
	AudioSystem    *mAudio;
	SoundEffect    *mShotSound;
	SoundEffect    *mDeathSound;
	SoundEffect    *mHitSound;

	float          mSnapshotTimer;
	float          mSendTimer;
	float          mSimulationAccumulator;

	Tmx::Map       *mTmxMap;

	std::vector<std::string> mGameMessages;

	Entity *getMe();
	void handlePlayerInput(float dt);
	void sendQueuedPackets(float dt);

	std::string    createUniqueEntId() const;

	float getPing(const std::string &name) const;

	void addGameMessage(const char *format, ...);
	void addGameMessage(const std::string &message);
	void trimGameMessages();

	void onGameState(const BitStream &stream);
	void onPlayerExit(const BitStream &stream);
	void onAllPlayersUpdate(const BitStream &stream);

	void addDefaultMap();
	void addGibs();
	void addHearts();
	
	void setHeartsDisplay(float health);
	
	PlayerState *getPlayerState(const std::string &id);

	// Make derived update method private so we can use serverTime as gameTime
	virtual void update(float gameTime, float dt);
};
