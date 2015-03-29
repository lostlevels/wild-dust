#pragma once

#include "Core/Precompiled.h"
#include "Game/Precompiled.h"
#include "Serialization/Precompiled.h"
#include "Network/Precompiled.h"

#include <SDL.h>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <enet/enet.h>

#ifdef BUILDING_SERVER
#define SERVER_API DLLEXPORT
#else
#define SERVER_API DLLIMPORT
#endif
