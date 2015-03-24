#pragma once

#include "PhysicsObjectType.h"

class Physics;
class PhysicsObject;

struct PhysicsCollision {
	PhysicsObject *otherObject;
	Vec2 edgeNormal;
};

class PHYSICS_API PhysicsObject {
public:
	PhysicsObject(Physics *phys, PhysicsObjectType type);
	~PhysicsObject();

	void setBox(int width, int height);

	Vec2 getPosition() const;
	void setPosition(const Vec2 &pos);

	int getWidth() const { return mWidth; }
	int getHeight() const { return mHeight; }

	Vec2 getVelocity() const;
	void setVelocity(const Vec2 &vel);

	float getFriction() const;
	void setFriction(float fric);

	std::vector<PhysicsCollision> getCollisions() const;

	void setUserData(void *data) { mUserData = data; }
	void *getUserData() const { return mUserData; }

private:
	void destroyFixture();

private:
	Physics *mPhys;
	b2Body *mBody;
	b2Fixture *mFixture;
	int mWidth;
	int mHeight;
	void *mUserData;
};
