#pragma once

#include "Core/Precompiled.h"

#include <SDL.h>
#include <SDL_ttf.h>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <enet/enet.h>

#ifdef BUILDING_RENDERER
#define RENDERER_API DLLEXPORT
#else
#define RENDERER_API DLLIMPORT
#endif
