#pragma once

#include "Core/Precompiled.h"

class GAME_API GibSpawner {
public:
	virtual void spawnGibs(const Vec3 &position, int amount) = 0;
};