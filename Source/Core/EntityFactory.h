#pragma once

#include <string>
#include "Entity.h"

class EntityFactory {
public:
	static Entity *createEntity(const std::string &id, const std::string &type,
		const std::string &owner, int sendMode = SEND_ALWAYS);
};