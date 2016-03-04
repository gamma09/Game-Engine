#pragma once

#ifdef WIN32
	#pragma once
	#define _CRT_SECURE_NO_WARNINGS 1

	#define WIN32_LEAN_AND_MEAN
	#define WIN32_EXTRA_LEAN
	#include <Windows.h>

	#include <GL\glew.h>

	#pragma comment (lib, "OpenGL32.lib")
#endif