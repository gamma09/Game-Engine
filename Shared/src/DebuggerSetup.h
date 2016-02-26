#pragma once

#ifdef _DEBUG

	#define WIN32_LEAN_AND_MEAN
	#define WIN32_EXTRA_LEAN
	#include <Windows.h>
	#include <stdio.h>

	static char errorBuffer[1024];
	#define out(A,...); sprintf_s(errorBuffer,A,##__VA_ARGS__); OutputDebugString(errorBuffer);

#else
	#define out(A,...); ;
#endif