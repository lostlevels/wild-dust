#pragma once

#include "Core/Precompiled.h"

#include <Box2D/Box2D.h>

#ifdef BUILDING_PHYSICS
#define PHYSICS_API DLLEXPORT
#else
#define PHYSICS_API DLLIMPORT
#endif
