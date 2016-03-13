#include <stdio.h>
#include <string.h>
#include <new>
#include <utility>

#include <GameAssert.h>

#include "mem.h"
#include "heap.h"
#include "block.h"

// Cuz this is turned off in stdlib for C++ (and we don't want to include all the junk that comes with Windows.h just for this)
#define max(a,b)    (((a) > (b)) ? (a) : (b))

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
void Mem::createVariableBlockHeap( Heap*& newHeap, unsigned int heapSize )
{
	Mem::instance().privCreateVariableBlockHeap( newHeap, heapSize );
}

void Mem::createFixBlockHeap( Heap*& newHeap, unsigned int numBlocks, unsigned int sizePerBlock )
{
	Mem::instance().privCreateFixedBlockHeap( newHeap, numBlocks, sizePerBlock );
}

void Mem::destroyHeap( Heap* inHeap )
{
	Mem::instance().privDestroyHeap( inHeap );
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
	globalTrackingBlockHead( nullptr ),
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

void Mem::privCreateVariableBlockHeap( Heap*& newHeap, unsigned int heapSize )
{
	GameAssert( heapSize > 0 );

	LowLevelHeap lowLevelHeap( heapSize );

	// We've have to move it into itself (sounds kinda weird and kinky at the same time) to increase speed (using this magic thing called the cache)
	newHeap = (Heap*) lowLevelHeap.Allocate( sizeof( VariableBlockHeap ) );
	newHeap = ( Heap* ) new( newHeap ) VariableBlockHeap( heapSize, std::move( lowLevelHeap ), this );

	if( this->heapHead != 0 )
		this->heapHead->prevHeap = newHeap;

	newHeap->nextHeap = this->heapHead;
	this->heapHead = newHeap;

	this->memInfo.currHeapCount++;
	this->memInfo.peakHeapCount = max( this->memInfo.peakHeapCount, this->memInfo.currHeapCount );
}

void Mem::privCreateFixedBlockHeap( Heap*& newHeap, unsigned int numBlocks, unsigned int sizePerBlock )
{
	GameAssert( numBlocks > 0 );
	GameAssert( sizePerBlock > 0 );

	LowLevelHeap lowLevelHeap( numBlocks * sizePerBlock + sizeof( FixedBlockHeap ) + sizeof( LowLevelUsedBlock ));
	
	// We've have to move it into itself (sounds kinda weird and kinky at the same time) to increase speed (using this magic thing called the cache)
	newHeap = (Heap*) lowLevelHeap.Allocate( sizeof( FixedBlockHeap ) );
	newHeap = ( Heap* ) new( newHeap ) FixedBlockHeap( numBlocks, sizePerBlock, std::move( lowLevelHeap ), this );

	

	if( this->heapHead != 0 )
		this->heapHead->prevHeap = newHeap;

	newHeap->nextHeap = this->heapHead;
	this->heapHead = newHeap;

	this->memInfo.currHeapCount++;
	this->memInfo.peakHeapCount = max( this->memInfo.peakHeapCount, this->memInfo.currHeapCount );
}

void Mem::privDestroyHeap( Heap* inHeap )
{
	GameAssert( inHeap != 0 );

	TrackingBlock* block = inHeap->GetHeapTrackingHead();

	while( block != 0 )
	{
		if( block->gNext != 0 )
			block->gNext->gPrev = block->gPrev;

		if( block->gPrev != 0 )
			block->gPrev->gNext = block->gNext;

		block = block->hNext;
	}

	this->memInfo.currNumAlloc -= inHeap->heapInfo.currNumAlloc;
	this->memInfo.currBytesUsed -= inHeap->heapInfo.currBytesUsed;
	this->memInfo.currHeapCount--;

	if( inHeap->nextHeap != 0 )
		inHeap->nextHeap->prevHeap = inHeap->prevHeap;

	if( inHeap->prevHeap != 0 )
		inHeap->prevHeap->nextHeap = inHeap->nextHeap;

	if( this->heapHead == inHeap )
		this->heapHead = inHeap->nextHeap;

	// We have to rip the low level heap out of the heap data
	// This will cause the heap to be destroyed on return from this function
	LowLevelHeap lowLevelHeap( inHeap->RipOutLowLevelHeap() );

	// Need to silence the warning about not using it (we are, after all, "creating" the object in order to destroy it...)
	lowLevelHeap;
}
