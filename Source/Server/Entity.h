#pragma once

#include "Core/BitStream.h"

class SV_Entity {
public:
	SV_Entity();
	virtual ~SV_Entity();

	virtual void writeToStream(BitStream &stream);
	virtual void readFromStream(const BitStream &stream);

	virtual void update(float dt);
};
