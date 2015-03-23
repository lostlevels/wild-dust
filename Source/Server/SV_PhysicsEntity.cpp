#include "Precompiled.h"
#include "SV_PhysicsEntity.h"
#include "Server.h"

SV_PhysicsEntity::SV_PhysicsEntity(Server *server, bool canMove) : SV_Entity(server) {
	mWorld = server->getPhysicsWorld();

	b2BodyDef bdef;
	bdef.type = canMove ? b2_dynamicBody : b2_staticBody;
	bdef.fixedRotation = true;
	mBody = mWorld->CreateBody(&bdef);

	mFixture = NULL;

	mWidth = 0;
	mHeight = 0;
}

SV_PhysicsEntity::~SV_PhysicsEntity() {
	mWorld->DestroyBody(mBody);
}

void SV_PhysicsEntity::writeToStream(BitStream &stream) {
	Vec2 pos = getPosition();
	stream.writeFloat(pos.x);
	stream.writeFloat(pos.y);

	stream.writeU16(mWidth);
	stream.writeU16(mHeight);
}

void SV_PhysicsEntity::setBox(int width, int height) {
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

void SV_PhysicsEntity::destroyFixture() {
	if (mFixture) {
		mBody->DestroyFixture(mFixture);
		mFixture = NULL;
	}
}

Vec2 SV_PhysicsEntity::getPosition() const {
	b2Vec2 pos = mBody->GetPosition();
	return Vec2((pos.x * 100.0f) - mWidth / 2, (pos.y * 100.0f) - mHeight / 2);
}

void SV_PhysicsEntity::setPosition(const Vec2 &pos) {
	mBody->SetTransform(b2Vec2(pos.x / 100.0f, pos.y / 100.0f), 0.0f);
}

Vec2 SV_PhysicsEntity::getVelocity() const {
	b2Vec2 lvel = mBody->GetLinearVelocity();
	return Vec2(lvel.x * 100.0f, lvel.y * 100.0f);
}

void SV_PhysicsEntity::setVelocity(const Vec2 &vel) {
	mBody->SetLinearVelocity(b2Vec2(vel.x / 100.0f, vel.y / 100.0f));
}

float SV_PhysicsEntity::getFriction() const {
	return mFixture->GetFriction() * 100.0f;
}

void SV_PhysicsEntity::setFriction(float fric) {
	mFixture->SetFriction(fric / 100.0f);
}

void SV_PhysicsEntity::limitSpeed(float speed) {
	Vec2 vel = getVelocity();
	Vec2 dir = glm::normalize(vel);
	float length = (float)vel.length();
	if (length > speed) {
		setVelocity(dir*speed);
	}
}
