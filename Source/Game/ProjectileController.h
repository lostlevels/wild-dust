#pragma once

#include "Core/Controller.h"

class GameContext;

class ProjectileController : public Controller {
public:
	ProjectileController(GameContext *context);
	virtual ~ProjectileController(){}

	virtual void control(Entity *e, float gameTime, float dt);
	virtual void onAttached(Entity *e);

private:
	GameContext *mContext;
	Vec3        mInitialPosition;
	bool        mPositionSet;

	virtual void onEnemyHit(Entity *hit){}
};
