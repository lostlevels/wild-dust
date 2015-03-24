#include "Precompiled.h"
#include "SV_Prop.h"

SV_Prop::SV_Prop(Server *server) : SV_Entity(server) {
	mSpriteName = "";
	mPosition = Vec2(0.0f, 0.0f);
	mSize = Vec2(0.0f, 0.0f);
}

SV_Prop::~SV_Prop() {

}

void SV_Prop::writeToStream(BitStream &stream) {
	stream.writeFloat(mPosition.x);
	stream.writeFloat(mPosition.y);
	stream.writeFloat(mSize.x);
	stream.writeFloat(mSize.y);
	stream.writeString(mSpriteName);
}

