#pragma once

#include "CL_Entity.h"
#include "Core/Clock.h"
#include <vector>

class Texture;

class CL_Sky : public CL_Entity {
public:
	CL_Sky(Client *client);
	virtual ~CL_Sky();

	virtual void draw();
	virtual void update(float dt);
private:
	Texture *mTexture;

	std::vector<Vec2> mStarPositions;
	float    mTwinkleTime;
};
