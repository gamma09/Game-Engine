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

	static void SetupMemorySystem();
	static void TeardownMemorySystem();

	// Create an Heap from the heap system
	static void createVariableBlockHeap( Heap*& newHeap, unsigned int heapSize, const char* heapName );
	static void createFixBlockHeap( Heap*& newHeap, unsigned int numBlocks, unsigned int sizePerBlock, const char* heapName );

	static void createFixBlockSystemHeap( Heap*& newHeap, unsigned int numBlocks, unsigned int sizePerBlock, const char* heapName );

	// Create an Heap from the heap system
	static void destroyHeap( Heap* inHeap );

	// get mem information
	static const MemInfo& getInfo();
	static MemReturnCode getHeapByAddr( Heap*& pHeap, const void* p );

#ifdef _DEBUG

	// Hidden -----------------  only accessible for debug mode
	// get heap information
	static Heap* DebugGetHeapHead();

	// get tracking Block head
	static TrackingBlock* DebugGetGlobalTrackingHead();

	static void DebugPrintLeakedMemory();

#endif

	// -----------------------------------------------------------------------------------
	// Add extra data or methods below this line
	// -----------------------------------------------------------------------------------
private:
	Mem();
	Mem( const Mem& mem ) = delete;
	Mem& operator=( const Mem& mem ) = delete;
	~Mem();

	void privCreateVariableBlockHeap( Heap*& newHeap, unsigned int heapSize, const char* heapName );
	void privCreateFixedBlockHeap( Heap*& newHeap, unsigned int numBlocks, unsigned int sizePerBlock, const char* heapName );
	void privDestroyHeap( Heap* inHeap );

private:
	// data -----------------------------------------------------------------------

	TrackingBlock* globalTrackingBlockHead;

	Heap*   heapHead;
	MemInfo memInfo;

	static Mem* instance;

	friend class VariableBlockHeap;
	friend class FixedBlockHeap;
};