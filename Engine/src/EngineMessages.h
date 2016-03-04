#pragma once

#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <Windows.h>


enum EngineMessages
{
	ENGINE_KILL_WINDOW = WM_USER,
};