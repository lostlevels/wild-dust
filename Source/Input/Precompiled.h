#pragma once

#include "Core/Precompiled.h"

#include <SDL.h>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <enet/enet.h>

#ifdef BUILDING_INPUT
#define INPUT_API DLLEXPORT
#else
#define INPUT_API DLLIMPORT
#endif
