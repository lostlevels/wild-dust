#pragma once

#include <string>

//
// TODO: Move to a shared game module
//

struct PlayerState {
	std::string name;
	std::string type;
	float       ping;
	float       health;
	int         kills;
	int         deaths;
	int         team;

	PlayerState() : name(""), type(""), ping(0), health(10), kills(0), deaths(0), team(0) {

	}
};
