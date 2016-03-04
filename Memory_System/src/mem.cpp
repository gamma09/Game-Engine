#include <stdio.h>
#include <GameAssert.h>
#include <string.h>
#include <new>

#include "mem.h"
#include "heap.h"
#include "block.h"

// --------------------------------------------------------------
// MemInfo methods
MemInfo::MemInfo() :
	currAllocIndex( 0 ),
	peakHeapCount( 0 ),
	currHeapCount( 0 ),
	peakNumAlloc( 0 ),
	currNumAlloc( 0 ),
	peakBytesUsed( 0 ),
	currBytesUsed( 0 )
{
	// Do nothing
}

// --------------------------------------------------------------
// Public method implementations
// --------------------------------------------------------------

// Create the heap.
MemReturnCode Mem::createVariableBlockHeap( Heap*& newHeap, int heapSize )
{
	return Mem::instance().privCreateVariableBlockHeap( newHeap, heapSize );
}

MemReturnCode Mem::createFixBlockHeap( Heap*& newHeap, int numBlocks, int sizePerBlock )
{
	return Mem::instance().privCreateFixedBlockHeap( newHeap, numBlocks, sizePerBlock );
}

MemReturnCode Mem::destroyHeap( Heap* inHeap )
{
	return Mem::instance().privDestroyHeap( inHeap );
}

MemReturnCode Mem::getHeapByAddr( Heap*& pHeap, const void* p )
{
	Mem& mem = Mem::instance();

	unsigned int pInt = (unsigned int) p;
	pHeap = mem.heapHead;
	while( pHeap != 0 && ( pInt < pHeap->heapInfo.heapStartAddr || pInt >= pHeap->heapInfo.heapEndAddr ) )
		pHeap = pHeap->nextHeap;

	if( pHeap == 0 )
		return Mem_ERROR_Invalid_Addr;
	else
		return Mem_OK;
}





// --------------------------------------------------------------
// Private method implementations
// --------------------------------------------------------------
Mem::Mem() :
#ifdef _DEBUG
	globalTrackingBlockHead( nullptr ),
#endif
	heapHead( nullptr ),
	memInfo()
{
	// Do nothing
}

Mem::~Mem()
{
	// Do nothing
}

Mem& Mem::instance()
{
	static Mem mem;

	return mem;
}

MemReturnCode Mem::privCreateVariableBlockHeap( Heap*& newHeap, int heapSize )
{
	GameAssert( heapSize > 0 );

	HANDLE hHeap = HeapCreate( 0, heapSize, 0 );
	if( hHeap == 0 )
		return Mem_ERROR_Heap_Create;

	newHeap = (Heap*) HeapAlloc( hHeap, 0, sizeof( VariableBlockHeap ) );
	newHeap = ( Heap* ) new( newHeap ) VariableBlockHeap( heapSize, hHeap, this );

	if( this->heapHead != 0 )
		this->heapHead->prevHeap = newHeap;

	newHeap->nextHeap = this->heapHead;
	this->heapHead = newHeap;

	this->memInfo.currHeapCount++;
	this->memInfo.peakHeapCount = max( this->memInfo.peakHeapCount, this->memInfo.currHeapCount );

	return Mem_OK;
}

MemReturnCode Mem::privCreateFixedBlockHeap( Heap*& newHeap, int numBlocks, int sizePerBlock )
{
	GameAssert( numBlocks > 0 );
	GameAssert( sizePerBlock > 0 );

	HANDLE hHeap = HeapCreate( 0, numBlocks * sizePerBlock + sizeof( Heap ), 0 );
	if( hHeap == 0 )
		return Mem_ERROR_Heap_Create;

	newHeap = (Heap*) HeapAlloc( hHeap, 0, sizeof( FixedBlockHeap ) );
	newHeap = ( Heap* ) new( newHeap ) FixedBlockHeap( numBlocks, sizePerBlock, hHeap, this );

	if( this->heapHead != 0 )
		this->heapHead->prevHeap = newHeap;

	newHeap->nextHeap = this->heapHead;
	this->heapHead = newHeap;

	this->memInfo.currHeapCount++;
	this->memInfo.peakHeapCount = max( this->memInfo.peakHeapCount, this->memInfo.currHeapCount );

	return Mem_OK;
}

MemReturnCode Mem::privDestroyHeap( Heap* inHeap )
{
	GameAssert( inHeap != 0 );

#ifdef _DEBUG
	TrackingBlock* block = inHeap->DebugGetHeapTrackingHead();

	while( block != 0 )
	{
		if( block->gNext != 0 )
			block->gNext->gPrev = block->gPrev;

		if( block->gPrev != 0 )
			block->gPrev->gNext = block->gNext;

		block = block->hNext;
	}
#endif

	this->memInfo.currNumAlloc -= inHeap->heapInfo.currNumAlloc;
	this->memInfo.currBytesUsed -= inHeap->heapInfo.currBytesUsed;
	this->memInfo.currHeapCount--;

	if( inHeap->nextHeap != 0 )
		inHeap->nextHeap->prevHeap = inHeap->prevHeap;

	if( inHeap->prevHeap != 0 )
		inHeap->prevHeap->nextHeap = inHeap->nextHeap;

	if( this->heapHead == inHeap )
		this->heapHead = inHeap->nextHeap;

	HeapDestroy( inHeap->winHeapHandle );

	return Mem_OK;
}
