#ifndef MEM_H
#define MEM_H

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

	// initialize the mem system
	// TODO why not do initialize at object construction, ye dum-dum? method is unnecessary
	static MemReturnCode initialize();
	static MemReturnCode destroy();

	// Create an Heap from the heap system
	static MemReturnCode createHeap(Heap *&newHeap, int heapSize, const char * const heapName);
	static MemReturnCode createFixBlockHeap(Heap *&newHeap, int numBlocks, int sizePerBlock, const char * const heapName);
   
	// Create an Heap from the heap system
	static MemReturnCode destroyHeap(Heap *inHeap);

	// get mem information
	static MemReturnCode getInfo(MemInfo &info);
	static MemReturnCode getHeapByAddr(Heap *&pHeap, const void *p);
	
// TODO uncomment this directive after milestone 1
//#ifdef _DEBUG

	// Hidden -----------------  only accessible for debug mode
	// get heap information
	static Heap *DebugGetHeapHead();

	// get tracking Block head
	static TrackingBlock *DebugGetGlobalTrackingHead();

// TODO uncomment this directive after milestone 1
//#endif

// -----------------------------------------------------------------------------------
// Add extra data or methods below this line
// -----------------------------------------------------------------------------------
private:
	Mem();
	Mem(const Mem& mem);
	Mem& operator=(const Mem& mem);
	~Mem();

	static Mem& instance();

	MemReturnCode privCreateHeap(Heap*& newHeap, int heapSize, const char * const heapName);
	MemReturnCode privCreateFixedBlockHeap(Heap *&newHeap, int numBlocks, int sizePerBlock, const char * const heapName);
	MemReturnCode privDestroyHeap(Heap* inHeap);

private:
	// data -----------------------------------------------------------------------

// TODO uncomment this directive after milestone 1
//#ifdef _DEBUG
	TrackingBlock	*globalTrackingBlockHead;
// TODO uncomment this directive after milestone 1
//#endif
	Heap			*heapHead;

	MemInfo			memInfo;
	bool			initialized; // TODO fucking useless shit (why not do it on construction, ye dum-dum?)

	friend class VariableBlockHeap;
	friend class FixedBlockHeap;
};

#endif //MEM_H