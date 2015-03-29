#pragma once

#include <glm/glm.hpp>

struct TransformSnapshot {
	float       time;
	Vec3        position;
	float       rotation;
	uint32_t    keys;
};
