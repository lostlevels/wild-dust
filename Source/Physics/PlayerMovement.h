#pragma once

class PhysicsObject;

class PHYSICS_API PlayerMovement {
public:
	PlayerMovement(PhysicsObject *playerObject);

	void moveLeft();
	void moveRight();
	void jump();

private:
	PhysicsObject *mPlayerObject;
};
