#pragma once

#include "ProjectileController.h"

class GAME_API LocalProjectileController : public ProjectileController {
public:
	LocalProjectileController(GameContext *context);
	virtual ~LocalProjectileController(){}
private:
	virtual void onEnemyHit(Entity *hit);
};
