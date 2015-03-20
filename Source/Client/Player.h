#pragma once

#include "Entity.h"

class Texture;

class CL_Player : public CL_Entity {
public:
	CL_Player(Client *client);
	virtual ~CL_Player();

	virtual void writeToStream(BitStream &stream);
	virtual void readFromStream(const BitStream &stream);

	virtual void update(float dt);

	virtual void draw();

private:
	Texture *mGrass;
	Vec2 mPosition;
};
