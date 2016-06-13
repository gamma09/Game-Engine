#include <utility>
#include <new.h>  // for placement new
#include <math.h>
#include <string.h>

#include "heap.h"
#include "mem.h"
#include "block.h"
#include "GameAssert.h"
#include "StringUtils.h"
#include "Logger.h"

// Cuz this is turned off in stdlib for C++ (and we don't want to include all the junk that comes with Windows.h just for this)
#define max(a,b)    (((a) > (b)) ? (a) : (b))


static inline unsigned int get_alignment( Align align )
{
	return (unsigned int) align;
}



// --------------------------------------------------------------
// HeapInfo implementation
// --------------------------------------------------------------
HeapInfo::HeapInfo()
{
	// Do nothing
}

HeapInfo::HeapInfo( unsigned int size, unsigned int startAddr, const char* name )
	: heapStartAddr( startAddr ),
	heapEndAddr( startAddr + size ),
	totalHeapSize( size ),
	peakNumAlloc( 0 ),
	currNumAlloc( 0 ),
	peakBytesUsed( 0 ),
	currBytesUsed( 0 ),
	currBytesFree( size ),
	isSystemHeap( false )
{
	copy_string( this->heapName, name );
}

HeapInfo::HeapInfo( const HeapInfo& info )
{
	memcpy( this, &info, sizeof( HeapInfo ) );
}

HeapInfo& HeapInfo::operator=( const HeapInfo& info )
{
	memcpy( this, &info, sizeof( HeapInfo ) );

	return *this;
}

HeapInfo::~HeapInfo()
{
	// Do nothing
}


// --------------------------------------------------------------
// Heap implementation
// --------------------------------------------------------------

Heap::Heap( LowLevelHeap&& inLowLevelHeap, unsigned int size, Mem* _mem, const char* heapName )
	: lowLevelHeap( std::move( inLowLevelHeap ) ),
	nextHeap( nullptr ),
	prevHeap( nullptr ),
	mem( _mem ),
	heapInfo( size, ( unsigned int ) this, heapName )
{
}

Heap::~Heap()
{
#ifdef _DEBUG
	Logger::Log_Debug( "Destroying Heap: %s", this->heapInfo.heapName );
	this->DebugPrintHeapLeaks();
#endif
}

void Heap::DebugPrintHeapLeaks() const
{
	for( TrackingBlock* curr = this->GetHeapTrackingHead(); curr; curr = curr->hNext )
	{
		Logger::Log_Debug( "  Live Object %d (%u bytes) created at: %s(%d)", curr->allocIndex, curr->allocSize, curr->fileName, curr->lineNum );
	}
}





// --------------------------------------------------------------
// VariableBlockHeap implementation
// --------------------------------------------------------------
VariableBlockHeap::VariableBlockHeap( unsigned int _size, LowLevelHeap&& inLowLevelHeap, Mem* _mem, const char* heapName )
	: Heap( std::move( inLowLevelHeap ), _size, _mem, heapName ),
	trackingBlockHead( nullptr )
{
}

void* VariableBlockHeap::alloc( size_t size, Align align, const char* name, int lineNum )
{
	// Ensure that the low level heap is valid before the allocation
#ifdef _DEBUG
	this->lowLevelHeap.CheckValidity();
	GameAssert( this->heapInfo.currNumAlloc + 1 == this->lowLevelHeap.CountAllocations() );
#endif

	unsigned int alignment = get_alignment( align );
	unsigned int alignedSize = size + sizeof( TrackingBlock ) + alignment - 1;

	void* ptr = this->lowLevelHeap.Allocate( alignedSize );
	TrackingBlock* block = new(ptr) TrackingBlock( name, lineNum );
	block->allocIndex = ++this->mem->memInfo.currAllocIndex;
	block->allocSize = alignedSize;

	block->gNext = this->mem->globalTrackingBlockHead;
	if( this->mem->globalTrackingBlockHead != 0 )
		this->mem->globalTrackingBlockHead->gPrev = block;
	this->mem->globalTrackingBlockHead = block;

	block->hNext = this->trackingBlockHead;
	if( this->trackingBlockHead != 0 )
		this->trackingBlockHead->hPrev = block;
	this->trackingBlockHead = block;

	this->heapInfo.currNumAlloc++;
	this->heapInfo.peakNumAlloc = max( this->heapInfo.currNumAlloc, this->heapInfo.peakNumAlloc );

	this->heapInfo.currBytesUsed += alignedSize;
	this->heapInfo.peakBytesUsed = max( this->heapInfo.currBytesUsed, this->heapInfo.peakBytesUsed );
	this->heapInfo.currBytesFree -= alignedSize;

	// Have we exceeded the amount of memory specified for heap size?
	GameAssert( ( this->heapInfo.totalHeapSize - sizeof( VariableBlockHeap ) ) >= this->heapInfo.currBytesUsed );

	this->mem->memInfo.currBytesUsed += alignedSize;
	this->mem->memInfo.peakBytesUsed = max( this->mem->memInfo.currBytesUsed, this->mem->memInfo.peakBytesUsed );

	this->mem->memInfo.currNumAlloc++;
	this->mem->memInfo.peakNumAlloc = max( this->mem->memInfo.peakNumAlloc, this->mem->memInfo.currNumAlloc );

	void* p = block + 1;

	void* alignedAddr = (void*) ( ( (unsigned int) p + alignment - 1 ) & ~( alignment - 1 ) );
	TrackingBlock** topPtr = (TrackingBlock**) ( (size_t) alignedAddr - sizeof( TrackingBlock* ) );
	*topPtr = block;

	// Make sure the low level heap is valid after the allocation
#ifdef _DEBUG
	this->lowLevelHeap.CheckValidity();
	GameAssert( this->heapInfo.currNumAlloc + 1 == this->lowLevelHeap.CountAllocations() );
#endif

	return alignedAddr;
}

void VariableBlockHeap::free( void* p )
{
	// Make sure the heap is valid before the free
#ifdef _DEBUG
	this->lowLevelHeap.CheckValidity();
	GameAssert( this->heapInfo.currNumAlloc + 1 == this->lowLevelHeap.CountAllocations() );
#endif

	TrackingBlock** topPtr = (TrackingBlock**) ( (size_t) p - sizeof( TrackingBlock*) );
	TrackingBlock* block = *topPtr;

	if( block->gNext != 0 )
		block->gNext->gPrev = block->gPrev;
	if( block->gPrev != 0 )
		block->gPrev->gNext = block->gNext;

	if( block->hNext != 0 )
		block->hNext->hPrev = block->hPrev;
	if( block->hPrev != 0 )
		block->hPrev->hNext = block->hNext;

	this->heapInfo.currBytesUsed -= block->allocSize;
	this->heapInfo.currNumAlloc--;

	this->mem->memInfo.currBytesUsed -= block->allocSize;
	this->mem->memInfo.currNumAlloc--;

	this->heapInfo.currBytesFree += block->allocSize;

	if( this->trackingBlockHead == block )
		this->trackingBlockHead = block->hNext;

	if( this->mem->globalTrackingBlockHead == block )
		this->mem->globalTrackingBlockHead = block->gNext;

	this->lowLevelHeap.Free( block );

	// Make sure the heap is valid after the free
#ifdef _DEBUG
	this->lowLevelHeap.CheckValidity();
	GameAssert( this->heapInfo.currNumAlloc + 1 == this->lowLevelHeap.CountAllocations() );
#endif
}

const char* VariableBlockHeap::GetHeapTypeString() const
{
	return "VariableBlockHeap";
}


TrackingBlock* VariableBlockHeap::GetHeapTrackingHead() const
{
	return this->trackingBlockHead;
}



// --------------------------------------------------------------
// FixedBlockHeap implementation
// --------------------------------------------------------------
FixedBlockHeap::FixedBlockHeap( unsigned int numBlocks, unsigned int _blockSize, LowLevelHeap&& heap, Mem* _mem, const char* heapName )
	: Heap( std::move( heap ), sizeof( LowLevelUsedBlock ) + numBlocks * max( (int) ceil( _blockSize / 4.0f ) * 4, sizeof( FreeBlock ) ), _mem, heapName ),
	blockSize( max( (int) ceil( _blockSize / 4.0f ) * 4, sizeof( FreeBlock ) ) )
{
	this->blocks = this->lowLevelHeap.Allocate( this->blockSize * numBlocks );
	// Malloc should have returned memory that was 4-byte aligned - just make sure this is so
	GameAssert( ((size_t) this->blocks & 3) == 0 );
	this->alignedFreeHead = (FreeBlock*) this->blocks;
	FreeBlock* curr = this->alignedFreeHead;

	for( unsigned int i = 0; i < numBlocks - 1; i++ )
	{
		curr->next = (FreeBlock*) ( (unsigned int) curr + this->blockSize );
		curr = curr->next;
	}

	curr->next = 0;
}

void* FixedBlockHeap::alloc( size_t size, Align align, const char*, int )
{
	GameAssert( size <= this->blockSize );
	GameAssert( align == ALIGN_4 );

	align;
	size;

	// Are we out of blocks?
	GameAssert( this->alignedFreeHead != 0 );

	void* p = this->alignedFreeHead;
	this->alignedFreeHead = this->alignedFreeHead->next;

	this->heapInfo.currBytesUsed += this->blockSize;
	this->heapInfo.peakBytesUsed = max( this->heapInfo.currBytesUsed, this->heapInfo.peakBytesUsed );

	this->heapInfo.currNumAlloc++;
	this->heapInfo.peakNumAlloc = max( this->heapInfo.currNumAlloc, this->heapInfo.peakNumAlloc );

	this->mem->memInfo.currBytesUsed += this->blockSize;
	this->mem->memInfo.peakBytesUsed = max( this->mem->memInfo.currBytesUsed, this->mem->memInfo.peakBytesUsed );

	this->mem->memInfo.currNumAlloc++;
	this->mem->memInfo.peakNumAlloc = max( this->mem->memInfo.currNumAlloc, this->mem->memInfo.peakNumAlloc );

	this->mem->memInfo.currAllocIndex++;

	return p;
}

void FixedBlockHeap::free( void* p )
{
	FreeBlock* block = (FreeBlock*) p;
	block->next = this->alignedFreeHead;
	this->alignedFreeHead = block;

	this->heapInfo.currBytesUsed -= this->blockSize;
	this->heapInfo.currNumAlloc--;

	this->mem->memInfo.currBytesUsed -= this->blockSize;
	this->mem->memInfo.currNumAlloc--;
}

const char* FixedBlockHeap::GetHeapTypeString() const
{
	return "FixedBlockHeap";
}
