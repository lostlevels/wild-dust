#include "Precompiled.h"
#include "EntityFactory.h"
#include "Core/Entity.h"
#include "Core/EntityRepresentation.h"
#include "Core/Logger.h"
#include "Game/GameContext.h"
#include "Game/PlayerController.h"
#include "Game/RemotePlayerController.h"
#include <map>

static Entity defaultEntity("", "notype", "", SEND_ALWAYS, {{18, 32}, "../Content/Textures/Characters/Cowboy.png", {0, 0, 18, 32}});

static std::map<std::string, Entity> entityMap = {
	{"cowboy", Entity("", "cowboy", "", SEND_ALWAYS, {{18 * 2, 32 * 2}, "../Content/Textures/Characters/Cowboy.png", {0, 0, 18, 32},
		{
			// Anims
			{"walk", {6, 9, 9, .25f}},
			{"jump", {5, 5, 5, 1}},
			{"shoot", {10, 13, 13, .25f}},
		}
	})},

	{"bandit", Entity("", "bandit", "", SEND_ALWAYS, {{24, 64}, "../Content/Textures/Characters/Bandit.png", {0, 0, 24, 64},
		{
			// Anims
			{"walk", {5, 7, 7,.25f}},
			{"jump", {4, 4, 4, 1}},
			{"shoot", {8, 9, 9, .25f}},
		}
	})},

	{"barista", Entity("", "barista", "", SEND_ALWAYS, {{11*2, 32*2}, "../Content/Textures/Characters/Barista.png", {0, 0, 11, 32},
		{
			// Anims
			{"walk", {1, 3, 3,.25f}},
			{"jump", {0, 0, 0, 1}},
			{"shoot", {4, 5, 5, .25f}},
		}
	})},

	{"cop", Entity("", "cop", "", SEND_ALWAYS, {{15*2, 32*2}, "../Content/Textures/Characters/Cop.png", {0, 0, 15, 32},
		{
			// Anims
			{"walk", {1, 3, 3,.25f}},
			{"jump", {0, 0, 0, 1}},
			{"shoot", {4, 5, 5, .25f}},
		}
	})},

	{"doc", Entity("", "doc", "", SEND_ALWAYS, {{10*2, 32*2}, "../Content/Textures/Characters/Doc.png", {0, 0, 10, 32},
		{
			// Anims
			{"walk", {1, 3, 3,.25f}},
			{"jump", {0, 0, 0, 1}},
			{"shoot", {4, 5, 5, .25f}},
		}
	})}
};


Entity *EntityFactory::createLocalEntity(GameContext *context, const std::string &id, const std::string &type, const std::string &owner) {
	Entity *e = EntityFactory::createEntity(context, id, type, owner);
	e->setLocal(true);
	e->setController(new PlayerController(context));
	return e;
}

Entity *EntityFactory::createRemoteEntity(GameContext *context, const std::string &id, const std::string &type, const std::string &owner) {
	Entity *e = EntityFactory::createEntity(context, id, type, owner);
	e->setLocal(false);
	e->setController(new RemotePlayerController(context));
	return e;
}

Entity *EntityFactory::createEntity(GameContext *context, const std::string &id, const std::string &type, const std::string &owner) {
	Entity *e = new Entity();

	// Entity doesn't have any special mem allocated stuff that we need to worry about during copying
	if (entityMap.find(type) == entityMap.end())
		*e = defaultEntity;
	else
		*e = entityMap[type];

	// Since we copied over values from entityMap, need to set true id and owner
	e->setId(id);
	e->setOwner(owner);

	return e;
}
