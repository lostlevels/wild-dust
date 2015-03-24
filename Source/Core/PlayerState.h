#pragma once

#include <string>

//
// TODO: Move to a shared game module
//

struct PlayerState {
	std::string name;
	std::string type;
	float       ping;
	int         kills;
	int         deaths;
	int         team;

	PlayerState() : name(""), type(""), ping(0), kills(0), deaths(0), team(0) {

	}
};
