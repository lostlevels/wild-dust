#pragma once

typedef uint32_t EntityID;

// Client-to-server commands
enum {
	NETCMD_CTS_PLAYER_INPUT
};

// Server-to-client commands
enum {
	NETCMD_STC_WORLD_SNAPSHOT
};

enum {
	BTN_MOVE_LEFT = (1<<0),
	BTN_MOVE_RIGHT = (1<<1),
	BTN_JUMP = (1<<2)
};

#pragma pack(1)
struct PlayerInput {
	uint8_t buttonMask; // combination of BTN_*
};
#pragma pack()
