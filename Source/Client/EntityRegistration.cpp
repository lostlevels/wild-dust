#include "Precompiled.h"
#include "World.h"
#include "CL_Player.h"
#include "CL_Projectile.h"
#include "CL_Prop.h"

#define ENTITY_FACTORY(x) [](Client *client){return new x(client); }

void RegisterClientEntityTypes(ClientWorld *world) {
	world->registerEntityType(ENTITY_PLAYER, ENTITY_FACTORY(CL_Player));
	world->registerEntityType(ENTITY_PROJECTILE, ENTITY_FACTORY(CL_Projectile));
	world->registerEntityType(ENTITY_PROP, ENTITY_FACTORY(CL_Prop));
}