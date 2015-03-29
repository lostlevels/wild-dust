#include "Precompiled.h"
#include "StarsEntity.h"
#include "WorldCollision.h"
#include <glm/gtx/norm.hpp>

StarsEntity::StarsEntity(const std::string &id, const EntityRepresentation &rep) :
	Entity(id, "stars", "", SEND_NEVER, rep) {

	mStars.resize(100);
	for (auto &star : mStars) {
		Vec3 position(0.0f);
		position.x = rand() % 1280;
		position.y = rand() % 720;
		star.centerPosition = position;
		star.scaleSpeed = Vec2(0, (rand()%100)/100.0f * 2 + 1.0f);
	}
}

void StarsEntity::update(float gameTime, float dt) {
	Entity::update(gameTime, dt);

	float baseSize = 8;
	float maxExtraSize = 4;
	for (auto &star : mStars) {
		star.scaleSpeed.x += dt * star.scaleSpeed.y * .15f;
		float extraSize = sinf(star.scaleSpeed.x * 3.1419265f) * maxExtraSize;
		star.size = Vec2(baseSize + extraSize);
	}
}
