#pragma once

#include "Core/Precompiled.h"

#ifdef BUILDING_SHARED
#define SHARED_API DLLEXPORT
#else
#define SHARED_API DLLIMPORT
#endif
