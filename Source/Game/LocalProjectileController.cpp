#include "Precompiled.h"
#include "LocalProjectileController.h"
#include "Core/Entity.h"
#include "GameContext.h"

// TODO Put projectile stats somewhere else

LocalProjectileController::LocalProjectileController(GameContext *context) : ProjectileController(context) {

}

void LocalProjectileController::onEnemyHit(Entity *hit) {
	// Send message that someone was hit
}