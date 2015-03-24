#pragma once

struct Recti {
	int32_t x;
	int32_t y;
	int32_t w;
	int32_t h;
	bool operator ==(const Recti &other) const {
		return x == other.x && y == other.y && w == other.w && h == other.h;
	}
};

struct Rectf {
	float x;
	float y;
	float w;
	float h;
};
