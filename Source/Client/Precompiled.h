#pragma once

#ifdef BUILDING_CLIENT
#define CLIENT_API __declspec(dllexport)
#else
#define CLIENT_API __declspec(dllimport)
#endif
