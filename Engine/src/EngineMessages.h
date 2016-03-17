#pragma once

#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <Windows.h>
#include <GameAssert.h>

#define BEGIN_ENGINE_MESSAGES enum EngineMessages { Begin = WM_USER - 1,
#define DECLARE_ENGINE_MESSAGE( msg ) msg,
#define END_ENGINE_MESSAGES };



BEGIN_ENGINE_MESSAGES



DECLARE_ENGINE_MESSAGE( WM_ENGINE_DESTROY_WINDOW )




END_ENGINE_MESSAGES
