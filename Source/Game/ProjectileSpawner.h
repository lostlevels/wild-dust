#pragma once

#include "Core/Precompiled.h"
#include <string>

class ProjectileSpawner {
public:
	// Rotation assumes pointing towards x axis is 0 and grows ccw.
	virtual void spawnProjectile(const std::string &type, const std::string &owner, const Vec3 &position, float rotation) = 0;
};