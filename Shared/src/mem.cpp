#include <stdio.h>
#include <string.h>
#include <new>
#include <utility>
#include <atomic>

#include "mem.h"
#include "heap.h"
#include "block.h"
#include "GameAssert.h"
#include "Logger.h"

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

Mem* Mem::instance = nullptr;

// --------------------------------------------------------------
// Public method implementations
// --------------------------------------------------------------

void Mem::SetupMemorySystem()
{
	GameAssert( Mem::instance == nullptr );
	Mem::instance = new Mem();
}

void Mem::TeardownMemorySystem()
{
	GameAssert( Mem::instance );

#ifdef _DEBUG
	Mem::instance->DebugPrintLeakedMemory();
#endif

	delete Mem::instance;
	Mem::instance = nullptr;
}

// Create the heap.
void Mem::createVariableBlockHeap( Heap*& newHeap, unsigned int heapSize, const char* heapName )
{
	GameAssert( Mem::instance );
	Mem::instance->privCreateVariableBlockHeap( newHeap, heapSize, heapName );
}

void Mem::createFixBlockHeap( Heap*& newHeap, unsigned int numBlocks, unsigned int sizePerBlock, const char* heapName )
{
	GameAssert( Mem::instance );
	Mem::instance->privCreateFixedBlockHeap( newHeap, numBlocks, sizePerBlock, heapName );
}

void Mem::createFixBlockSystemHeap( Heap*& newHeap, unsigned int numBlocks, unsigned int sizePerBlock, const char* heapName )
{
	Mem::createFixBlockHeap( newHeap, numBlocks, sizePerBlock, heapName );
	newHeap->heapInfo.isSystemHeap = true;
}

void Mem::destroyHeap( Heap* inHeap )
{
	GameAssert( Mem::instance );
	Mem::instance->privDestroyHeap( inHeap );
}

MemReturnCode Mem::getHeapByAddr( Heap*& pHeap, const void* p )
{
	if( Mem::instance )
	{
		unsigned int pInt = (unsigned int) p;
		pHeap = Mem::instance->heapHead;
		while( pHeap != 0 && ( pInt < pHeap->heapInfo.heapStartAddr || pInt >= pHeap->heapInfo.heapEndAddr ) )
		{
			pHeap = pHeap->nextHeap;
		}

		return pHeap ? Mem_OK : Mem_ERROR_Invalid_Addr;
	}
	else
	{
		return Mem_ERROR_Invalid_Addr;
	}
}





// --------------------------------------------------------------
// Private method implementations
// --------------------------------------------------------------
Mem::Mem() :
	globalTrackingBlockHead( nullptr ),
	heapHead( nullptr ),
	memInfo()
{
}

Mem::~Mem()
{
}

const MemInfo& Mem::getInfo()
{
	GameAssert( instance );
	return instance->memInfo;
}

void Mem::privCreateVariableBlockHeap( Heap*& newHeap, unsigned int heapSize, const char* heapName )
{
	GameAssert( heapSize > 0 );
	LowLevelHeap lowLevelHeap( heapSize );

	// We've have to move it into itself (sounds kinda weird and kinky at the same time) to increase speed (using this magic thing called the cache)
	newHeap = (Heap*) lowLevelHeap.Allocate( sizeof( VariableBlockHeap ) );
	newHeap = ( Heap* ) new( newHeap ) VariableBlockHeap( heapSize, std::move( lowLevelHeap ), this, heapName );

	if( this->heapHead )
	{
		this->heapHead->prevHeap = newHeap;
	}

	newHeap->nextHeap = this->heapHead;
	this->heapHead = newHeap;

	this->memInfo.currHeapCount++;
	this->memInfo.peakHeapCount = max( this->memInfo.peakHeapCount, this->memInfo.currHeapCount );

	std::atomic_thread_fence( std::memory_order_release );
}

void Mem::privCreateFixedBlockHeap( Heap*& newHeap, unsigned int numBlocks, unsigned int sizePerBlock, const char* heapName )
{
	GameAssert( numBlocks > 0 );
	GameAssert( sizePerBlock > 0 );

	LowLevelHeap lowLevelHeap( numBlocks * sizePerBlock + sizeof( FixedBlockHeap ) + sizeof( LowLevelUsedBlock ));
	
	// We've have to move it into itself (sounds kinda weird and kinky at the same time) to increase speed (using this magic thing called the cache)
	newHeap = (Heap*) lowLevelHeap.Allocate( sizeof( FixedBlockHeap ) );
	newHeap = ( Heap* ) new( newHeap ) FixedBlockHeap( numBlocks, sizePerBlock, std::move( lowLevelHeap ), this, heapName );

	if( this->heapHead )
	{
		this->heapHead->prevHeap = newHeap;
	}

	newHeap->nextHeap = this->heapHead;
	this->heapHead = newHeap;

	this->memInfo.currHeapCount++;
	this->memInfo.peakHeapCount = max( this->memInfo.peakHeapCount, this->memInfo.currHeapCount );

	std::atomic_thread_fence( std::memory_order_release );
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

#ifdef _DEBUG

Heap* Mem::DebugGetHeapHead()
{
	GameAssert( Mem::instance );
	return Mem::instance->heapHead;
}

TrackingBlock* Mem::DebugGetGlobalTrackingHead()
{
	GameAssert( Mem::instance );
	return Mem::instance->globalTrackingBlockHead;
}

void Mem::DebugPrintLeakedMemory()
{
	GameAssert( Mem::instance );
	Logger::Log_Debug( "--- MEMORY LEAK ANALYSIS ---" );
	for( const Heap* heap = Mem::instance->heapHead; heap; heap = heap->nextHeap )
	{
		if( !heap->heapInfo.isSystemHeap )
		{
			Logger::Log_Debug( "Live %s: %s (%u live objects using %u bytes)", heap->GetHeapTypeString(), heap->heapInfo.heapName, heap->heapInfo.currNumAlloc, heap->heapInfo.currBytesUsed );
			heap->DebugPrintHeapLeaks();
		}
	}
}

#endif
