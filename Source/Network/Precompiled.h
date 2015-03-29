#pragma once

#include "Core/Precompiled.h"

#include <enet/enet.h>

#ifdef BUILDING_NETWORK
#define NETWORK_API DLLEXPORT
#else
#define NETWORK_API DLLIMPORT
#endif
