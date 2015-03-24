#pragma once

#include "CL_Entity.h"

class Texture;

class CL_Prop : public CL_Entity {
public:
	CL_Prop(Client *client);
	virtual ~CL_Prop();

	virtual void readFromStream(const BitStream &stream);

	virtual void draw();

private:
	void loadSprite(const std::string &spriteName);
	void discardTexture();

private:
	std::string mSpriteName;
	Texture *mTexture;
	Vec2 mPosition;
	Vec2 mSize;
};