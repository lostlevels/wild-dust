#pragma once

#include "Core/Precompiled.h"

#ifdef BUILDING_GAME
#define GAME_API DLLEXPORT
#else
#define GAME_API DLLIMPORT
#endif
