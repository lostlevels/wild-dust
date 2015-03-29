#pragma once

#include "Core/Controller.h"

class GameContext;

class PlayerController : public Controller {
public:
	// Later on PlayerController(world / collision map / )
	PlayerController(GameContext *context);
	virtual ~PlayerController(){}

	virtual void control(Entity *e, float gameTime, float dt);
private:
	GameContext *mContext;
};
