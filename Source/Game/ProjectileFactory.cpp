#include "Precompiled.h"
#include "ProjectileFactory.h"
#include "Core/Entity.h"
#include "Core/EntityRepresentation.h"
#include "Game/ProjectileController.h"
#include "Game/LocalProjectileController.h"
#include "Game/GameContext.h"
#include <map>

static Entity defaultProjectile("", "bullet", "", SEND_ENTER, {{16, 16}, "../Content/Textures/Misc/Bullet.png", {0, 0, 8, 8}});

Entity *ProjectileFactory::createRemoteProjectile(GameContext *context, const std::string &id, const std::string &type, const std::string &owner, const Vec3 &velocity) {
	auto e = ProjectileFactory::createProjectile(context, id, type, owner, velocity);
	// Snap to current time based on velocity ?
	e->setController(new ProjectileController(context));
	return e;
}

Entity *ProjectileFactory::createLocalProjectile(GameContext *context, const std::string &id, const std::string &type, const std::string &owner, const Vec3 &velocity) {
	auto e =  ProjectileFactory::createProjectile(context, id, type, owner, velocity);
	e->setController(new LocalProjectileController(context));
	return e;
}

Entity *ProjectileFactory::createProjectile(GameContext *context, const std::string &id, const std::string &type, const std::string &owner, const Vec3 &velocity) {
	Entity *e = new Entity();
	*e = defaultProjectile;

	e->setVelocity(velocity);
	e->setId(id);
	e->setType(type);
	e->setOwner(owner);

	return e;
}
