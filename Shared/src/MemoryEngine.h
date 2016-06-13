#pragma once

#include "Enums.h"
#include "mem.h"
#include "heap.h"
#include "global.h"
#include "block.h"

// Uncomment the following line to turn off the memory system
// #define DONT_OVERRIDE_NEW

#ifdef DONT_OVERRIDE_NEW

#include <stdlib.h>
#define new(heap, align) new
#define newArray( type, count, heap, align ) ((type)*) malloc( sizeof( type ) * count )
#define newPtr( heap, align, size ) malloc( size )

#else

#define new(heap, align) new((heap), (align), __FILE__, __LINE__)
#define newArray( type, count, heap, align )  ( (type*) Allocate( sizeof( type ) * (count), heap, align, __FILE__, __LINE__ ) )
#define newPtr( heap, align, size )  ( Allocate( (size), (heap), (align), __FILE__, __LINE__ ) )

#endif