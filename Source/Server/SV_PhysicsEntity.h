#pragma once

#include "SV_Entity.h"

class SV_PhysicsEntity : public SV_Entity {
public:
	SV_PhysicsEntity(Server *server, bool canMove);
	virtual ~SV_PhysicsEntity();

	virtual void writeToStream(BitStream &stream);

	void setBox(int width, int height);

	Vec2 getPosition() const;
	void setPosition(const Vec2 &pos);

	int getWidth() const { return mWidth; }
	int getHeight() const { return mHeight; }

	Vec2 getVelocity() const;
	void setVelocity(const Vec2 &vel);

	float getFriction() const;
	void setFriction(float fric);

	void limitSpeed(float speed);

	b2Body *getBody() const { return mBody; }

private:
	void destroyFixture();

private:
	b2World *mWorld;
	b2Body *mBody;
	b2Fixture *mFixture;
	int mWidth;
	int mHeight;
};
