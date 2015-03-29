#pragma once

#include <string>
#include "Core/Entity.h"

class GameContext;

class GAME_API EntityFactory {
public:
	static Entity *createLocalEntity(GameContext *context, const std::string &id, const std::string &type,
		const std::string &owner);
	static Entity *createRemoteEntity(GameContext *context, const std::string &id, const std::string &type,
		const std::string &owner);
private:
	static Entity *createEntity(GameContext *context, const std::string &id, const std::string &type,
		const std::string &owner);
};