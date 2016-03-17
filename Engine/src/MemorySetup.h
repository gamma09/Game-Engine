#pragma once

#include <MemoryEngine.h>
#include "GlobalHeaps.h"


#ifdef DONT_OVERRIDE_NEW
#define new(heap, align) new
#else
#define new(heap, align) new((heap), (align), __FILE__, __LINE__)
#define Allocate( heap, align, size ) Allocate( (size), (heap), (align), __FILE__, __LINE__ )
#endif