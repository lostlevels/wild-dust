#include "Precompiled.h"
#include "PlayerController.h"
#include "Core/Entity.h"
#include "Core/InputBase.h"
#include "Core/World.h"
#include "Core/WorldCollision.h"
#include "Core/Collision.h"
#include "ProjectileSpawner.h"
#include "GameContext.h"

// PlayerController(Input, Spawner / world ????)
PlayerController::PlayerController(GameContext *context) :
	mContext(context) {

}

// Collision offsets, make 1 pixel free for each corner
// *------*
// |      |
// |      |
// *------*
//

static int collideVertically(Entity *e, WorldCollision *collider, const glm::vec3 &from, glm::vec3 &to) {
	if (fabsf(from.y - to.y) < .001f) return 0;

	int collision = 0;
	auto &rep = e->getRepresentation();
	Vec2 bottomOffsets[] = {
		Vec2(1, rep.size.y - 1),
		Vec2(rep.size.x / 2, rep.size.y - 1),
		Vec2(rep.size.x - 2, rep.size.y - 1),
	};

	Vec2 topOffsets[] = {
		Vec2(1, 0),
		Vec2(rep.size.x / 2, 0),
		Vec2(rep.size.x - 2, 0), 
	};

	if (to.y > from.y) {
		for (Vec2 &offset : bottomOffsets) {
			while (collider->collides(to.x + offset.x, to.y + offset.y)) {
				to.y--;
				collision |= COL_BOTTOM;
			}
		}
	}
	else {
		for (Vec2 &offset : topOffsets) {
			while (collider->collides(to.x + offset.x, to.y + offset.y)) {
				to.y++;
				collision |= COL_TOP;
			}
		}
	}

	return collision;
}


static int collideHorizontally(Entity *e, WorldCollision *collider, const glm::vec3 &from, glm::vec3 &to) {
	if (fabsf(from.x - to.x) < .001f) return 0;

	int collision = 0;
	auto &rep = e->getRepresentation();

	Vec2 leftOffsets[] = {
		Vec2(0, 1),
		Vec2(0, rep.size.y / 2),
		Vec2(0, rep.size.y - 2)
	};

	Vec2 rightOffsets[] = {
		Vec2(rep.size.x - 1, 1),
		Vec2(rep.size.x - 1, rep.size.y / 2),
		Vec2(rep.size.x - 1, rep.size.y - 2)
	};

	if (to.x > from.x) {
		for(Vec2 &offset : rightOffsets) {
			while (collider->collides(to.x + offset.x, to.y + offset.y)) {
				to.x--;
				collision |= COL_RIGHT;
			}
		}
	}
	else {
		for(Vec2 &offset : leftOffsets) {
			while (collider->collides(to.x + offset.x, to.y + offset.y)) {
				to.x++;
				collision |= COL_LEFT;
			}
		}
	}
	return collision;
}

static bool isTouchingGround(Entity *e, WorldCollision *collider, const Vec3 &position) {
	auto &rep = e->getRepresentation();
	Vec2 bottomOffsets[] = {
		Vec2(1, rep.size.y - 1),
		Vec2(rep.size.x / 2, rep.size.y - 1),
		Vec2(rep.size.x - 2, rep.size.y - 1),
	};

	for (Vec2 &offset : bottomOffsets) {
		if (collider->collides(position.x + offset.x, position.y + offset.y + 2))
			return true;
	}

	return false;
}

// TODO: Fix terrible collision
void PlayerController::control(Entity *e, float gameTime, float dt) {
	auto input = mContext->getInput();
	if (!input) return;

	auto collider = mContext->getCollision();
	int buttons = input->getButtons();
	int newButtons = input->getNewButtons();
	float speed = 250;
	bool shooting = e->isAnimating() && e->getAnimation() == "shoot";

	auto accel = e->getAccel();
	accel.y = 1500.0f;
	e->setAccel(accel);

	auto velocity = e->getVelocity();
	auto position = e->getPosition();
	Vec3 lastPosition = position;

	if (buttons & BTN_MOVE_LEFT) {
		position.x -= speed * dt;
		e->setFlip(1);
	}
	if (buttons & BTN_MOVE_RIGHT) {
		position.x += speed * dt;
		e->setFlip(0);
	}

	// cheap hack fix
	static bool jumping = false;
	bool touchingGround = isTouchingGround(e, collider, position);
	if (buttons & BTN_JUMP && touchingGround && !jumping) {
		velocity.y += -700.0f;
		jumping = true;
	}
	else if (touchingGround && velocity.y > -.00001f) {
		jumping = false;
	}

	if (buttons & (BTN_MOVE_RIGHT | BTN_MOVE_LEFT) && !shooting && (e->getAnimation() != "walk" || !e->isAnimating())) {
		e->animate("walk", -1);
	}
	else if (!(buttons & (BTN_MOVE_RIGHT | BTN_MOVE_LEFT)) && e->isAnimating() && !shooting) {
		e->stopAnimating();
	}

	if (mContext->getProjectileSpawner() && buttons & BTN_ATTACK && !shooting) {
		mContext->getProjectileSpawner()->spawnProjectile("bullet", e->getOwner(), e->getCenteredPosition(), e->getFlip() > 0 ? 180 : 0);
		e->animate("shoot", 1);
	}

	velocity.y += accel.y * dt;
	position.y += velocity.y * dt;

	int collision = 0;
	Vec3 to = position;
	if (fabsf(to.y - lastPosition.y) >= fabsf(to.x - lastPosition.y)) {
		to.x = lastPosition.x;
		collision |= collideVertically(e, collider, lastPosition, to);
		to.x = position.x;
		collideHorizontally(e, collider, lastPosition, to);
		position = to;
	}
	else {
		to.y = lastPosition.y;
		collision |= collideHorizontally(e, collider, lastPosition, to);
		to.y = position.y;
		collision |= collideVertically(e, collider, lastPosition, to);
		position = to;
	}

	if (collision & (COL_BOTTOM | COL_TOP)) {
		velocity.y = 0;
	}

	e->setVelocity(velocity);
	e->setPosition(position);
}