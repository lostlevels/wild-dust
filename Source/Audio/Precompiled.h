#pragma once

#include "Core/Precompiled.h"

#ifdef BUILDING_AUDIO
#define AUDIO_API DLLEXPORT
#else
#define AUDIO_API DLLIMPORT
#endif
