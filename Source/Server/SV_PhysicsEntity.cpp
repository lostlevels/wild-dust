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
	shape.SetAsBox(width / 2.0f, height / 2.0f);

	b2FixtureDef fdef;
	fdef.shape = &shape;
	fdef.density = 1.0f;
	mFixture = mBody->CreateFixture(&fdef);

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
	return Vec2(pos.x - mWidth / 2, pos.y - mHeight / 2);
}

void SV_PhysicsEntity::setPosition(const Vec2 &pos) {
	mBody->SetTransform(b2Vec2(pos.x, pos.y), 0.0f);
}

Vec2 SV_PhysicsEntity::getVelocity() const {
	b2Vec2 lvel = mBody->GetLinearVelocity();
	return Vec2(lvel.x, lvel.y);
}

void SV_PhysicsEntity::setVelocity(const Vec2 &vel) {
	mBody->SetLinearVelocity(b2Vec2(vel.x, vel.y));
}

float SV_PhysicsEntity::getFriction() const {
	return mFixture->GetFriction();
}

void SV_PhysicsEntity::setFriction(float fric) {
	mFixture->SetFriction(fric);
}
