#pragma once

#include <assert.h>


#define GameAssert(a) assert(a)

#if defined(_DEBUG) || defined(DEBUG)

#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <Windows.h>
#include <crtdbg.h>
	
	#define GameVerify(a) assert(a)
#define GameCheckFatal( a, msg ) _ASSERT_EXPR( a, msg )

#else

	#include <stdio.h>
	#include <stdlib.h>

	#define GameVerify(a) (a)
	#define GameCheckFatal( a, msg ) (((a) || 0 > fprintf( stderr, "Fatal error! %s(%d): %s\n", __FILE__, __LINE__, msg )) ? (void) 0 : exit( EXIT_FAILURE ))

#endif