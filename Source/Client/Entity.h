#pragma once

#include "Core/BitStream.h"

class CL_Entity {
public:
	CL_Entity();
	virtual ~CL_Entity();

	virtual void writeToStream(BitStream &stream);
	virtual void readFromStream(const BitStream &stream);

	virtual void update(float dt);
};
