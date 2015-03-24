#include "Precompiled.h"
#include "Physics.h"
#include "PhysicsObject.h"

Physics::Physics() {
	b2Vec2 gravity(0, 10);
	mWorld = new b2World(gravity);
}

Physics::~Physics() {
	delete mWorld;
}

PhysicsObject *Physics::createObject(PhysicsObjectType type) {
	return new PhysicsObject(this, type);
}

void Physics::step(float dt) {
	mWorld->Step(dt, 10, 10);
}

