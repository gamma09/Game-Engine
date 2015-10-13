#pragma once

#ifdef WIN32
	#pragma once
	#define _CRT_SECURE_NO_WARNINGS 1

	#define WIN32_LEAN_AND_MEAN 1
	#include <Windows.h>

	#ifdef _DEBUG
		#pragma comment (lib, "GLFW_Debug.lib")
		#pragma comment (lib, "GL3W_Debug.lib")
	#else
		#pragma comment (lib, "GLFW_Release.lib")
		#pragma comment (lib, "GL3W_Release.lib")
	#endif

	#pragma comment (lib, "OpenGL32.lib")
#endif