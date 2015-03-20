#include "Precompiled.h"
#include "Player.h"
#include "World.h"
#include "Projectile.h"

#define ENTITY_FACTORY(x) [](Server *server){return new x(server); }

void RegisterServerEntityTypes(ServerWorld *world) {
	world->registerEntityType("Player", ENTITY_FACTORY(SV_Player));
	world->registerEntityType("Projectile", ENTITY_FACTORY(SV_Projectile));
}