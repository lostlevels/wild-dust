#pragma once

#include "Core/Precompiled.h"

class Damager {
public:
	virtual void applyDamage(const std::string &id, float amount) = 0;
};