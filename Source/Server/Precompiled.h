#pragma once

#ifdef BUILDING_SERVER
#define SERVER_API __declspec(dllexport)
#else
#define SERVER_API __declspec(dllimport)
#endif
