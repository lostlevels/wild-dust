#include "Precompiled.h"
#include "Player.h"
#include "Server.h"

SV_Player::SV_Player(Server *server) : SV_Entity(server) {
	createPhysicsBody();
}

void SV_Player::createPhysicsBody() {
	b2World *physWorld = mServer->getPhysicsWorld();
	
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.fixedRotation = true;
	mPhysBody = physWorld->CreateBody(&bodyDef);

	b2PolygonShape shape;
	shape.SetAsBox(16, 16);

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &shape;
	fixtureDef.density = 1.0f;

	mPhysBody->CreateFixture(&fixtureDef);
}

void SV_Player::update(float dt) {
}

void SV_Player::writeToStream(BitStream &stream) {
	b2Vec2 position = mPhysBody->GetPosition();
	stream.writeFloat(position.x-16);
	stream.writeFloat(position.y-16);
}

void SV_Player::moveLeft() {
	b2Vec2 vel = mPhysBody->GetLinearVelocity();
	vel.x -= 100;
	mPhysBody->SetLinearVelocity(vel);
}

void SV_Player::moveRight() {
	b2Vec2 vel = mPhysBody->GetLinearVelocity();
	vel.x += 100;
	mPhysBody->SetLinearVelocity(vel);
}

void SV_Player::shoot() {

}

void SV_Player::jump() {

}
