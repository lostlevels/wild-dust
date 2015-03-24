#pragma once

#include "SV_Entity.h"

class SV_Prop : public SV_Entity {
public:
	SV_Prop(Server *server);
	virtual ~SV_Prop();

	virtual void writeToStream(BitStream &stream);

	std::string mSpriteName;
	Vec2 mPosition;
	Vec2 mSize;
};
