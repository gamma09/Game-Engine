#include <GameAssert.h>
#include <MemoryEngine.h>

#include "GlobalHeaps.h"

TemporaryHeap* TemporaryHeap::instance;

void TemporaryHeap::Create()
{
	GameAssert(instance == 0);

	instance = new TemporaryHeap();
}

void TemporaryHeap::Destroy()
{
	GameAssert(instance != 0);

	delete instance;
	instance = 0;
}

Heap* TemporaryHeap::Instance()
{
	GameAssert(instance != 0);

	return instance->heap;
}

// We might need A LOT of memory, depending on what is using this heap...
// Throw down with 128 MB first...
#define TEMPORARY_HEAP_SIZE 4096
// 128 * 1024 * 1024

TemporaryHeap::TemporaryHeap()
{
	
	GameVerify( Mem_OK == Mem::createHeap(this->heap, TEMPORARY_HEAP_SIZE, "Temporary Allocations Heap") );
}

TemporaryHeap::~TemporaryHeap()
{
	GameVerify( Mem_OK == Mem::destroyHeap(this->heap) );
}