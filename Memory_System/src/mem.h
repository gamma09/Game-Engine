#pragma once

#include "Enums.h"

#define UNUSED( x ) x
#define MEM_VERSION 1.0

// forward declaration
class Heap;
class TrackingBlock;

// Return codes
enum MemReturnCode
{
	Mem_OK,
	Mem_ERROR_Heap_Create,
	Mem_ERROR_Illegal_Heap,
	Mem_ERROR_Mem_Not_Initialized,
	Mem_ERROR_Invalid_Addr,
	Mem_Dword = 0x7fffffff
};

// mem info structure
struct MemInfo
{
	unsigned int currAllocIndex;
	unsigned int peakHeapCount;
	unsigned int currHeapCount;
	unsigned int peakNumAlloc;
	unsigned int currNumAlloc;
	unsigned int peakBytesUsed;
	unsigned int currBytesUsed;

	MemInfo();
};

// create a singleton
class Mem
{
public:
	// Public Interface ------------------------------   for the customers

	// Create an Heap from the heap system
	static MemReturnCode createVariableBlockHeap( Heap*& newHeap, int heapSize );
	static MemReturnCode createFixBlockHeap( Heap*& newHeap, int numBlocks, int sizePerBlock );

	// Create an Heap from the heap system
	static MemReturnCode destroyHeap( Heap* inHeap );

	// get mem information
	inline static const MemInfo& getInfo() { return Mem::instance().memInfo; }
	static MemReturnCode getHeapByAddr( Heap*& pHeap, const void* p );

#ifdef _DEBUG

	// Hidden -----------------  only accessible for debug mode
	// get heap information
	inline static Heap* DebugGetHeapHead() { return Mem::instance().heapHead; }

	// get tracking Block head
	inline static TrackingBlock* DebugGetGlobalTrackingHead() { return Mem::instance().globalTrackingBlockHead; }

#endif

	// -----------------------------------------------------------------------------------
	// Add extra data or methods below this line
	// -----------------------------------------------------------------------------------
private:
	Mem();
	Mem( const Mem& mem ) = delete;
	Mem& operator=( const Mem& mem ) = delete;
	~Mem();

	static Mem& instance();

	MemReturnCode privCreateVariableBlockHeap( Heap*& newHeap, int heapSize );
	MemReturnCode privCreateFixedBlockHeap( Heap*& newHeap, int numBlocks, int sizePerBlock );
	MemReturnCode privDestroyHeap( Heap* inHeap );

private:
	// data -----------------------------------------------------------------------

#ifdef _DEBUG
	TrackingBlock* globalTrackingBlockHead;
#endif

	Heap*   heapHead;
	MemInfo memInfo;

	friend class VariableBlockHeap;
	friend class FixedBlockHeap;
};