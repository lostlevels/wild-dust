#include "Precompiled.h"
#include "EntityFactory.h"
#include "Entity.h"
#include "EntityRepresentation.h"
#include <map>

static Entity defaultEntity("", "notype", "", SEND_ALWAYS, {"../Content/Textures/Characters/Cowboy.png", 0, 0, 18, 32});

static std::map<std::string, Entity> entityMap = {
	{"cowboy", Entity("", "cowboy", "", SEND_ALWAYS, {"../Content/Textures/Characters/Cowboy.png", 0, 0, 18, 32})}
};

Entity *EntityFactory::createEntity(const std::string &id, const std::string &type, const std::string &owner, int sendMode) {
	Entity *e = new Entity();

	// Entity doesn't have any special mem allocated stuff that we need to worry about during copying
	if (entityMap.find(type) == entityMap.end()) {
		*e = defaultEntity;
	}
	else {
		*e = entityMap[type];
	}

	// Since we copied over values from entityMap, need to set true id and type
	e->setId(id);
	e->setSendMode(sendMode);
	e->setOwner(owner);

	return e;
}