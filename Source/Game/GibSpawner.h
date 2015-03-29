#pragma once

#include "Core/Precompiled.h"

class GibSpawner {
public:
	virtual void spawnGibs(const Vec3 &position, int amount) = 0;
};