#include "Precompiled.h"
#include "World.h"
#include "SV_Player.h"
#include "SV_Projectile.h"

#define ENTITY_FACTORY(x) [](Server *server){return new x(server); }

void RegisterServerEntityTypes(ServerWorld *world) {
	world->registerEntityType("Player", ENTITY_FACTORY(SV_Player));
	world->registerEntityType("Projectile", ENTITY_FACTORY(SV_Projectile));
}