#pragma once

#include "Core/Precompiled.h"
#include "Game/Precompiled.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifdef BUILDING_SERIALIZATION
#define SERIALIZATION_API DLLEXPORT
#else
#define SERIALIZATION_API DLLIMPORT
#endif