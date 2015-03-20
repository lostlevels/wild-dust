#pragma once

#include "Entity.h"

class SV_Player : public SV_Entity {
public:
	SV_Player(Server *server);

	virtual void update(float dt);

	virtual void writeToStream(BitStream &stream);

public:
	Vec2 mPosition;
};