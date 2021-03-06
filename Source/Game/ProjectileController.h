#pragma once

#include "Core/Controller.h"

class GameContext;

class GAME_API ProjectileController : public Controller {
public:
	ProjectileController(GameContext *context);
	virtual ~ProjectileController(){}

	virtual void control(Entity *e, float gameTime, float dt);
	virtual void onAttached(Entity *e);

protected:
	GameContext *mContext;
	Vec3        mInitialPosition;
	bool        mPositionSet;

	virtual void onEnemyHit(Entity *hit){}
};
