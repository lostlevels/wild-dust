#include "Precompiled.h"
#include "Player.h"
#include "World.h"

#define ENTITY_FACTORY(x) [](Client *client){return new x(client); }

void RegisterClientEntityTypes(ClientWorld *world) {
	world->registerEntityType("Player", ENTITY_FACTORY(CL_Player));
}