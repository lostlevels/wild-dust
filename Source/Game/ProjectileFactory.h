#pragma once

#include <string>
#include "Core/Entity.h"

class GameContext;

class ProjectileFactory {
public:
	static Entity *createRemoteProjectile(GameContext *context, const std::string &id, const std::string &type, const std::string &owner, const Vec3 &velocity);
	static Entity *createLocalProjectile(GameContext *context, const std::string &id, const std::string &type, const std::string &owner, const Vec3 &velocity);
private:
	static Entity *createProjectile(GameContext *context, const std::string &id, const std::string &type, const std::string &owner, const Vec3 &velocity);
};