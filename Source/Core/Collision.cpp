#include "Precompiled.h"
#include "Collision.h"
#include "Entity.h"

// TODO: http://www.gamedev.net/page/resources/_/technical/game-programming/swept-aabb-collision-detection-and-response-r3084

bool Collision::collides(Entity *a, Entity *b) {
	Vec3 positionA = a->getPosition();
	Vec3 positionB = b->getPosition();
	Vec2 sizeA = a->getRepresentation().size;
	Vec2 sizeB = b->getRepresentation().size;

	return positionA.x <= positionB.x + sizeB.x && positionA.x + sizeA.x >= positionB.x &&
		positionA.y <= positionB.y + sizeB.y && positionA.y + sizeA.y >= positionB.y;
}

// Check note by hypernewbie
float Collision::collidesBullet(Entity *bullet, Entity *other, const Vec3 &from, const Vec3 &to) {
		struct Box
	{
		// position of top-left corner
		float x, y;

		// dimensions
		float w, h;

		// velocity
		float vx, vy;
	};
	Box b1, b2;

	Vec2 aSize = bullet->getRepresentation().size;
	Vec2 bSize = other->getRepresentation().size;

	Vec3 bPosition = other->getPosition();

	b1.vx = to.x - from.x;
	b1.vy = to.y - from.y;
	b1.x = from.x;
	b1.y = from.y;
	b1.w = aSize.x;
	b1.h = aSize.y;

	b2.x = bPosition.x;
	b2.y = bPosition.y;
	b2.w = bSize.x;
	b2.h = bSize.y;

	float xInvEntry, yInvEntry;
    float xInvExit, yInvExit;

    // find the distance between the objects on the near and far sides for both x and y
    if (b1.vx > 0.0f)
    {
        xInvEntry = b2.x - (b1.x + b1.w);
        xInvExit = (b2.x + b2.w) - b1.x;
    }
    else
    {
        xInvEntry = (b2.x + b2.w) - b1.x;
        xInvExit = b2.x - (b1.x + b1.w);
    }

    if (b1.vy > 0.0f)
    {
        yInvEntry = b2.y - (b1.y + b1.h);
        yInvExit = (b2.y + b2.h) - b1.y;
    }
    else
    {
        yInvEntry = (b2.y + b2.h) - b1.y;
        yInvExit = b2.y - (b1.y + b1.h);
    }

    // find time of collision and time of leaving for each axis (if statement is to prevent divide by zero)
    float xEntry, yEntry;
    float xExit, yExit;

    if (b1.vx == 0.0f)
    {
        xEntry = -std::numeric_limits<float>::infinity();
        xExit = std::numeric_limits<float>::infinity();
    }
    else
    {
        xEntry = xInvEntry / b1.vx;
        xExit = xInvExit / b1.vx;
    }

    if (b1.vy == 0.0f)
    {
        yEntry = -std::numeric_limits<float>::infinity();
        yExit = std::numeric_limits<float>::infinity();
    }
    else
    {
        yEntry = yInvEntry / b1.vy;
        yExit = yInvExit / b1.vy;
    }

     // find the earliest/latest times of collision
    float entryTime = std::max(xEntry, yEntry);
    float exitTime = std::min(xExit, yExit);
	
#define NO_COLLISION 10000.0f
    // if there was no collision
    if (entryTime > exitTime) return NO_COLLISION;
    if (xEntry < 0.0f && yEntry < 0.0f) return NO_COLLISION;
	if (xEntry < 0.0f) {
		// Check that the bounding box started overlapped or not.
		//if (s.max.x < t.min.x || s.min.x > t.max.x) return 1.0f;

		if (b1.x + b1.w < b2.x || b1.x > b2.x + b2.w) return NO_COLLISION;
	}
	if (yEntry < 0.0f) {
		// Check that the bounding box started overlapped or not.
		//if (s.max.y < t.min.y || s.min.y > t.max.y) return 1.0f;
			if (b1.y + b1.h < b2.y || b1.y > b2.y + b2.h) return NO_COLLISION;
	}

    // if ((entryTime > exitTime || xEntry < 0.0f) && (yEntry < 0.0f || xEntry > 1.0f || yEntry > 1.0f))
    // {
    //     return 1000.0f;
    // }
	// There was a collision

        // return the time of collision
        return entryTime;
}