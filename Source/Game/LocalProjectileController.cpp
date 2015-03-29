#include "Precompiled.h"
#include "LocalProjectileController.h"
#include "Core/Entity.h"
#include "GameContext.h"
#include "Damager.h"

LocalProjectileController::LocalProjectileController(GameContext *context) : ProjectileController(context) {

}

void LocalProjectileController::onEnemyHit(Entity *hit) {
	if (!mContext || !mContext->getDamager()) return;

	mContext->getDamager()->applyDamage(hit->getId(), 1);
}