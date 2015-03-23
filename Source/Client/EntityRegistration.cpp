#include "Precompiled.h"
#include "World.h"
#include "CL_Player.h"
#include "CL_Projectile.h"

#define ENTITY_FACTORY(x) [](Client *client){return new x(client); }

void RegisterClientEntityTypes(ClientWorld *world) {
	world->registerEntityType("Player", ENTITY_FACTORY(CL_Player));
	world->registerEntityType("Projectile", ENTITY_FACTORY(CL_Projectile));
}