#pragma once

#include <SDL.h>
#include <GL/glew.h>

#ifdef BUILDING_CLIENT
#define CLIENT_API __declspec(dllexport)
#else
#define CLIENT_API __declspec(dllimport)
#endif
