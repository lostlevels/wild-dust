#pragma once

#include "Core/Precompiled.h"

#include <enet/enet.h>
#include <Box2D/Box2D.h>

#ifdef BUILDING_SERVER
#define SERVER_API DLLEXPORT
#else
#define SERVER_API DLLIMPORT
#endif
