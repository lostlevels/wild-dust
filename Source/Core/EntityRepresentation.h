#pragma once

#include <map>
#include <string>
#include "SpriteAnimation.h"

struct EntityRepresentation {
	Vec2  size;
	std::string imageFile;
	Recti source;

	std::map<std::string, SpriteAnimation> animations;
};
