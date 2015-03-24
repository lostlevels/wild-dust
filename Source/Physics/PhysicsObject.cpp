#include "Precompiled.h"
#include "PhysicsObject.h"
#include "Physics.h"

PhysicsObject::PhysicsObject(Physics *phys, PhysicsObjectType type) {
	mPhys = phys;

	b2BodyDef bodyDef;
	bodyDef.fixedRotation = true;

	switch (type) {
	case PHYSICS_STATIC:
		bodyDef.type = b2_staticBody;
		break;
		
	case PHYSICS_KINEMATIC:
		bodyDef.type = b2_kinematicBody;
		break;

	case PHYSICS_DYNAMIC:
		bodyDef.type = b2_dynamicBody;
		break;
	}

	b2World *world = phys->getWorld();
	mBody = world->CreateBody(&bodyDef);

	mFixture = NULL;

	mWidth = 0;
	mHeight = 0;

	mUserData = NULL;
}

PhysicsObject::~PhysicsObject() {
	mPhys->getWorld()->DestroyBody(mBody);
}

void PhysicsObject::setBox(int width, int height) {
	destroyFixture();

	b2PolygonShape shape;
	shape.SetAsBox((width / 100.0f) / 2.0f, (height / 100.0f) / 2.0f);

	b2FixtureDef fdef;
	fdef.shape = &shape;
	fdef.density = 1.0f;
	fdef.friction = 1.0f;
	mFixture = mBody->CreateFixture(&fdef);
	mFixture->SetUserData(this);

	mWidth = width;
	mHeight = height;
}

void PhysicsObject::destroyFixture() {
	if (mFixture) {
		mBody->DestroyFixture(mFixture);
		mFixture = NULL;
	}
}

Vec2 PhysicsObject::getPosition() const {
	b2Vec2 pos = mBody->GetPosition();
	return Vec2((pos.x * 100.0f) - mWidth / 2, (pos.y * 100.0f) - mHeight / 2);
}

void PhysicsObject::setPosition(const Vec2 &pos) {
	mBody->SetTransform(b2Vec2((pos.x + mWidth / 2) / 100.0f, (pos.y + mHeight / 2) / 100.0f), 0.0f);
}

Vec2 PhysicsObject::getVelocity() const {
	b2Vec2 lvel = mBody->GetLinearVelocity();
	return Vec2(lvel.x * 100.0f, lvel.y * 100.0f);
}

void PhysicsObject::setVelocity(const Vec2 &vel) {
	mBody->SetLinearVelocity(b2Vec2(vel.x / 100.0f, vel.y / 100.0f));
}

float PhysicsObject::getFriction() const {
	return mFixture->GetFriction() * 100.0f;
}

void PhysicsObject::setFriction(float fric) {
	mFixture->SetFriction(fric / 100.0f);
}

std::vector<PhysicsCollision> PhysicsObject::getCollisions() const {
	std::vector<PhysicsCollision> cols;

	for (b2ContactEdge *edge = mBody->GetContactList(); edge; edge = edge->next) {
		b2Contact* contact = edge->contact;
		if (contact->IsTouching() == false) {
			continue;
		}

		b2WorldManifold man;
		contact->GetWorldManifold(&man);

		PhysicsCollision col;
		col.otherObject = reinterpret_cast<PhysicsObject*>(contact->GetFixtureA()->GetUserData());
		col.edgeNormal = Vec2(man.normal.x, man.normal.y);
		cols.push_back(col);
	}

	return cols;
}
