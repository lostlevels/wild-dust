#pragma once

#include "Entity.h"
#include "CameraTarget.h"

class Texture;

class CL_Player : public CL_Entity, public ICameraTarget {
public:
	CL_Player(Client *client);
	virtual ~CL_Player();

	virtual void readFromStream(const BitStream &stream);

	virtual void update(float dt);

	virtual void draw();

	virtual Vec2 ICameraTarget_getPosition() const;
	virtual Vec2 ICameraTarget_getSize() const;

private:
	Texture *mGrass;
	Vec2 mPosition;
};
