#pragma once

#include "Entity.h"
#include <vector>

#define MAX_GIBS 256

struct Star {
	Vec3 centerPosition;
	Vec2 size;
	Vec2 scaleSpeed;
};

class CORE_API StarsEntity : public Entity {
public:
	StarsEntity(const std::string &id, const EntityRepresentation &rep);
	virtual ~StarsEntity() {}

	virtual void update(float gameTime, float dt);

	virtual std::string getRenderType() const { return "stars"; }

	const std::vector<Star> &getStars() const { return mStars; }

private:
	std::vector<Star> mStars;
};
