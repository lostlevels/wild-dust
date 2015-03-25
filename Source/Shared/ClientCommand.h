#pragma once

#include "PlayerInput.h"
#include "Teams.h"

enum {
	CLIENT_CMD_INPUT,
	CLIENT_CMD_CHANGE_TEAM
};

struct ClientCommand {
	uint8_t type;
	PlayerInput input;
	Team desiredTeam;
};
