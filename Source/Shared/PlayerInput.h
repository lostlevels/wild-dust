#pragma once

enum {
	BTN_MOVE_LEFT = (1 << 0),
	BTN_MOVE_RIGHT = (1 << 1),
	BTN_JUMP = (1 << 2),
	BTN_ATTACK = (1 << 3)
};

#pragma pack(1)
struct PlayerInput {
	float serverTime;
	uint32_t sequenceIndex;
	uint8_t buttonMask; // combination of BTN_*
};
#pragma pack()