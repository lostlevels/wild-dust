#pragma once

typedef uint32_t EntityID;

// Server-to-client commands
enum {
	NETCMD_STC_PLAYER_IDENTIFY,
	NETCMD_STC_LOAD_MAP,
	NETCMD_STC_WORLD_SNAPSHOT
};

