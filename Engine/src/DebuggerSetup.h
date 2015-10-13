#pragma once

#include <Windows.h>
#include <stdio.h>

#ifdef _DEBUG

	static char errorBuffer[1024];
	#define out(A,...); sprintf_s(errorBuffer,A,##__VA_ARGS__); OutputDebugString(errorBuffer);

#else
	#define out(A,...); ;
#endif