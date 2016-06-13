#pragma once

#include <assert.h>


#define GameAssert(a) assert(a)
#define GameCheckWarning( a, msg ) Debugging::WarnIf( !(a), msg )

namespace Debugging
{
	void WarnIf( bool expr, const char* msg );
}

#if defined(_DEBUG) || defined(DEBUG)

#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <Windows.h>
#include <crtdbg.h>
	
	#define GameVerify(a) assert(a)
	#define GameCheckFatal( a, msg )  (void) ((!!(a)) || \
                (1 != _CrtDbgReport(_CRT_ASSERT, __FILE__, __LINE__, NULL, "%s", msg)) || \
                (_CrtDbgBreak(), 0))

#else

	#include <stdio.h>
	#include <stdlib.h>

	#define GameVerify(a) (a)
	#define GameCheckFatal( a, msg ) (((a) || 0 > fprintf( stderr, "Fatal error! %s(%d): %s\n", __FILE__, __LINE__, msg )) ? (void) 0 : exit( EXIT_FAILURE ))

#endif