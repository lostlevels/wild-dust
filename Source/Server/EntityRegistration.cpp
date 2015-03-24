#include "Precompiled.h"
#include "World.h"
#include "SV_Player.h"
#include "SV_Projectile.h"
#include "SV_Prop.h"

#define ENTITY_FACTORY(x) [](Server *server){return new x(server); }

void RegisterServerEntityTypes(ServerWorld *world) {
	world->registerEntityType(ENTITY_PLAYER, ENTITY_FACTORY(SV_Player));
	world->registerEntityType(ENTITY_PROJECTILE, ENTITY_FACTORY(SV_Projectile));
	world->registerEntityType(ENTITY_PROP, ENTITY_FACTORY(SV_Prop));
}