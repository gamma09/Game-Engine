#include <GameAssert.h>

#include "global.h"
#include "heap.h"

// operator new
void* operator new(size_t inSize, Heap *pHeap, Align align, const char * const inName, int lineNum)
{
	GameAssert(pHeap != 0);
	GameAssert(inName != 0);
	GameAssert(lineNum > 0);

	return pHeap->alloc(inSize, align, inName, lineNum);
}

// operator delete
void operator delete(void* p, Heap*, Align, const char* const, int)
{
	operator delete(p);
}

// operator delete
void operator delete(void *p)
{
	Heap* heap;
	if (Mem::getHeapByAddr(heap, p) == Mem_OK)
		heap->free(p);
	else
		free(p);
}

void* operator new[](size_t inSize, Heap* pHeap, Align align, const char* const inName, int lineNum)
{
	GameAssert(pHeap != 0);
	GameAssert(inName != 0);
	GameAssert(lineNum > 0);

	return pHeap->alloc(inSize, align, inName, lineNum);
}

void operator delete[](void *p, Heap*, Align, const char * const, int)
{
	operator delete[](p);
}

void operator delete[]( void *p )
{
	Heap* heap;
	if (Mem::getHeapByAddr(heap, p) == Mem_OK)
		heap->free(p);
	else
		free(p);
}