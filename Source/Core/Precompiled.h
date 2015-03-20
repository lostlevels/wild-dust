#pragma once

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN 1
#include <Windows.h>
#endif

#include <string>
#include <cstdint>
#include <cstdio>
#include <cstdarg>
#include <map>
#include <vector>

#include <glm/glm.hpp>

typedef glm::vec2 Vec2;

#ifdef _WIN32
#define DLLEXPORT __declspec(dllexport)
#define DLLIMPORT __declspec(dllimport)
#else
#define DLLEXPORT
#define DLLIMPORT
#endif

#ifdef BUILDING_CORE
#define CORE_API DLLEXPORT
#else
#define CORE_API DLLIMPORT
#endif

#include "Logger.h"
#include "Rect.h"
