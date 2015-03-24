#pragma once

#include "Teams.h"

enum {
	BTN_MOVE_LEFT = (1 << 0),
	BTN_MOVE_RIGHT = (1 << 1),
	BTN_JUMP = (1 << 2),
	BTN_ATTACK = (1 << 3)
};

#pragma pack(1)
struct PlayerInput {
	uint8_t buttonMask; // combination of BTN_*
};
#pragma pack()

enum {
	CLIENT_CMD_INPUT,
	CLIENT_CMD_CHANGE_TEAM
};

struct ClientCommand {
	uint8_t type;
	PlayerInput input;
	Team desiredTeam;
};
