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

	b2Vec2 points[4] = {
		b2Vec2(0.0f, 0.0f),
		b2Vec2(1.0f, 0.0f),
		b2Vec2(1.0f, 1.0f),
		b2Vec2(0.0f, 1.0f),
	};

	b2PolygonShape shape;
	shape.Set(points, 4);

	b2FixtureDef fdef;
	fdef.shape = &shape;
	fdef.density = 1.0f;
	fdef.friction = 1.0f;
	fdef.userData = this;
	mFixture = mBody->CreateFixture(&fdef);

	mBox.x = 0;
	mBox.y = 0;
	mBox.w = 100;
	mBox.h = 100;

	mUserData = NULL;
}

PhysicsObject::~PhysicsObject() {
	mPhys->getWorld()->DestroyBody(mBody);
}

void PhysicsObject::setBox(int width, int height) {
	setBox({ 0, 0, width, height });
}

void PhysicsObject::setBox(const Recti &box) {
	float left = (float)box.x / 100.0f;
	float top = (float)box.y / 100.0f;
	float right = left + (float)box.w / 100.0f;
	float bottom = top + (float)box.h / 100.0f;

	b2Vec2 points[4] = {
		b2Vec2(left, top),
		b2Vec2(right, top),
		b2Vec2(right, bottom),
		b2Vec2(left, bottom),
	};

	b2PolygonShape* shape = (b2PolygonShape*)mFixture->GetShape();
	shape->Set(points, 4);

	mBox = box;
}

void PhysicsObject::destroyFixture() {
	if (mFixture) {
		mBody->DestroyFixture(mFixture);
		mFixture = NULL;
	}
}

Vec2 PhysicsObject::getPosition() const {
	b2Vec2 pos = mBody->GetPosition();
	return Vec2(pos.x * 100.0f, pos.y * 100.0);
}

void PhysicsObject::setPosition(const Vec2 &pos) {
	mBody->SetTransform(b2Vec2(pos.x / 100.0f, pos.y / 100.0f), 0.0f);
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
