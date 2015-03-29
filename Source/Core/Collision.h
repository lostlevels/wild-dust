#pragma once

enum {
	COL_LEFT = 1,
	COL_RIGHT = 2,
	COL_TOP = 4,
	COL_BOTTOM = 8
};

class Entity;

class Collision {
public:
	static bool collides(Entity *a, Entity *b);
	static float collidesBullet(Entity *bullet, Entity *b, const Vec3 &from, const Vec3 &to);
};
