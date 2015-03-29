#pragma once

class WorldCollision {
public:
	virtual bool collides(float x, float y) const = 0;
};
