#include <GameAssert.h>
#include <MemoryEngine.h>

#include "GlobalHeaps.h"

TemporaryHeap* TemporaryHeap::instance = nullptr;

void TemporaryHeap::Create()
{
	GameAssert( instance == 0 );

	instance = new TemporaryHeap();
}

void TemporaryHeap::Destroy()
{
	GameAssert( instance != 0 );

	delete instance;
	instance = 0;
}

Heap* TemporaryHeap::Instance()
{
	GameAssert( instance != 0 );

	return instance->heap;
}

// We might need A LOT of memory, depending on what is using this heap...
// Throw down with 128 MB first...
#define TEMPORARY_HEAP_SIZE 134217728
// 128 * 1024 * 1024 = 134,217,728

TemporaryHeap::TemporaryHeap()
{

	Mem::createVariableBlockHeap( this->heap, TEMPORARY_HEAP_SIZE );
}

TemporaryHeap::~TemporaryHeap()
{
	Mem::destroyHeap( this->heap );
}

ConstantBufferHeap* ConstantBufferHeap::instance = nullptr;

void ConstantBufferHeap::Create()
{
	GameAssert( instance == nullptr );
	
	instance = new ConstantBufferHeap();
}

void ConstantBufferHeap::Destroy()
{
	GameAssert( instance != nullptr );

	delete instance;
	instance = nullptr;
}

Heap* ConstantBufferHeap::Instance()
{
	GameAssert( instance != nullptr );

	return instance->heap;
}

// Start with 2 pages of memory
#define CONSTANT_BUFFER_HEAP_SIZE 8192

ConstantBufferHeap::ConstantBufferHeap()
{
	Mem::createVariableBlockHeap( this->heap, CONSTANT_BUFFER_HEAP_SIZE );
}

ConstantBufferHeap::~ConstantBufferHeap()
{
	Mem::destroyHeap( this->heap );
}