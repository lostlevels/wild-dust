#include "Precompiled.h"
#include "RemotePlayerController.h"
#include "GameContext.h"
#include "ProjectileSpawner.h"
#include "Core/Entity.h"
#include "Core/InputBase.h"
#include "Core/World.h"

RemotePlayerController::RemotePlayerController(GameContext *context) :
	mContext(context) {

}

void RemotePlayerController::control(Entity *e, float gameTime, float dt) {
	float viewTime = gameTime - e->getViewDelay();
	auto snapshot = e->getSnapshot(viewTime);
	e->interpolate(viewTime);

	int buttons = snapshot.keys;
	bool shooting = e->isAnimating() && e->getAnimation() == "shoot";

	if (buttons & BTN_MOVE_LEFT) {
		e->setFlip(1);
	}
	if (buttons & BTN_MOVE_RIGHT) {
		e->setFlip(0);
	}

	if (buttons & (BTN_MOVE_RIGHT | BTN_MOVE_LEFT) && !shooting && (e->getAnimation() != "walk" || !e->isAnimating()))
		e->animate("walk", -1);
	else if (!(buttons & (BTN_MOVE_RIGHT | BTN_MOVE_LEFT)) && e->isAnimating() && !shooting)
		e->stopAnimating();
	
	if (buttons & BTN_ATTACK && !shooting) {
		e->animate("shoot", 1);
		if (mContext && mContext->getProjectileSpawner())
			mContext->getProjectileSpawner()->spawnProjectile("bullet", e->getOwner(), e->getCenteredPosition(), e->getFlip() > 0 ? 180 : 0);
	}
}