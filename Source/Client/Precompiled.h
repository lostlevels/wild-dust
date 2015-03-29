#pragma once

#include "Core/Precompiled.h"
#include "Renderer/Precompiled.h"

#include <SDL.h>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <enet/enet.h>

#ifdef BUILDING_CLIENT
#define CLIENT_API DLLEXPORT
#else
#define CLIENT_API DLLIMPORT
#endif
