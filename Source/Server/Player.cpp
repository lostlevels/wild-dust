#include "Precompiled.h"
#include "Player.h"
#include "Server.h"

#define PLAYER_WIDTH 32
#define PLAYER_HEIGHT 64

SV_Player::SV_Player(Server *server) : SV_Entity(server) {
	createPhysicsBody();

	mState = PLAYER_IDLE;
}

void SV_Player::createPhysicsBody() {
	b2World *physWorld = mServer->getPhysicsWorld();
	
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.fixedRotation = true;
	mPhysBody = physWorld->CreateBody(&bodyDef);

	b2PolygonShape shape;
	shape.SetAsBox(PLAYER_WIDTH / 2, PLAYER_HEIGHT / 2);

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &shape;
	fixtureDef.density = 1.0f;

	mPhysBody->CreateFixture(&fixtureDef);
}

void SV_Player::update(float dt) {
	b2Vec2 linearVel = mPhysBody->GetLinearVelocity();
	if (linearVel.y != 0) {
		mState = PLAYER_JUMPING;
	}
	else {
		if (linearVel.x != 0) {
			mState = PLAYER_MOVING;
		}
		else {
			mState = PLAYER_IDLE;
		}
	}
}

void SV_Player::writeToStream(BitStream &stream) {
	b2Vec2 position = mPhysBody->GetPosition();
	stream.writeFloat(position.x - PLAYER_WIDTH / 2);
	stream.writeFloat(position.y - PLAYER_HEIGHT / 2);
	stream.writeU16(PLAYER_WIDTH);
	stream.writeU16(PLAYER_HEIGHT);
	stream.writeAny(mState);
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
	b2Vec2 vel = mPhysBody->GetLinearVelocity();
	vel.y -= 50.0f;
	mPhysBody->SetLinearVelocity(vel);
}
