#pragma once

#include "Core/Controller.h"

class GameContext;

class RemotePlayerController : public Controller {
public:
	RemotePlayerController(GameContext *context);
	virtual ~RemotePlayerController(){}

	virtual void control(Entity *e, float gameTime, float dt);
private:
	GameContext *mContext;
};
