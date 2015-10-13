#pragma once

#include <MemoryEngine.h>
#include "GlobalHeaps.h"


#ifdef OVERRIDE_NEW
#define new(heap, align) new((heap), (align), __FILE__, __LINE__)
#else
#define new(heap, align) new
#endif