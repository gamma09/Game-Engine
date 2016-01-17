#pragma once

#include <cassert>

#define GameAssert(a) assert(a)

#if defined(_DEBUG) || defined(DEBUG)

	
	#define GameVerify(a) assert(a)

#else

	#define GameVerify(a) (a)

#endif