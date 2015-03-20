#pragma once

#include "Core/Precompiled.h"

#ifdef BUILDING_SERVER
#define SERVER_API DLLEXPORT
#else
#define SERVER_API DLLIMPORT
#endif
