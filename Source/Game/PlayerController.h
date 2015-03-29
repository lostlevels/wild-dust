#pragma once

#include "Core/Controller.h"

class GameContext;

class GAME_API PlayerController : public Controller {
public:
	PlayerController(GameContext *context);
	virtual ~PlayerController(){}

	virtual void control(Entity *e, float gameTime, float dt);
private:
	GameContext *mContext;
};
