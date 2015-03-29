#pragma once


class InputBase;
class ProjectileSpawner;
class World;
class WorldCollision;
class GibSpawner;

class GAME_API GameContext {
public:
	virtual InputBase *getInput() = 0;
	virtual ProjectileSpawner *getProjectileSpawner() = 0;
	virtual GibSpawner *getGibSpawner() = 0;
	virtual World *getWorld() = 0;
	virtual WorldCollision *getCollision() = 0;
};
