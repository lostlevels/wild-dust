#pragma once

class CORE_API WorldCollision {
public:
	virtual bool collides(float x, float y) const = 0;
};
