#pragma once

#include "PhysicsObjectType.h"

class PhysicsObject;

class PHYSICS_API Physics {
public:
	Physics();
	~Physics();
	
	PhysicsObject *createObject(PhysicsObjectType type);

	void step(float dt);

	b2World *getWorld() { return mWorld; }
	
private:
	b2World *mWorld;
};
