#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <new>

#include "mem.h"
#include "heap.h"
#include "block.h"

// --------------------------------------------------------------
// MemInfo methods
MemInfo::MemInfo() :
	currAllocIndex(0),
	peakHeapCount(0),
	currHeapCount(0),
	peakNumAlloc(0),
	currNumAlloc(0),
	peakBytesUsed(0),
	currBytesUsed(0)
{
	// Do nothing
}

// --------------------------------------------------------------
// Public method implementations
// --------------------------------------------------------------

// Create the heap.
MemReturnCode Mem::createHeap(Heap*& newHeap, int heapSize, const char * const heapName)
{
	return Mem::instance().privCreateHeap(newHeap, heapSize, heapName);
}

MemReturnCode Mem::createFixBlockHeap(Heap*& newHeap, int numBlocks, int sizePerBlock, const char * const heapName)
{
	return Mem::instance().privCreateFixedBlockHeap(newHeap, numBlocks, sizePerBlock, heapName);
}

// initialize the memory system  (Public method)
// TODO fucking useless shit, ditch the method entirely after milestone 1
MemReturnCode Mem::initialize()
{
	// Editorial comment, courtesy of Dave:
	// Caller: Let's initialize the SHIT out of this memory system.
	// Memory System: Alright, lets see, set initialized to true.
	// Memory System: OK! All done!
	// Caller: Thx d00d!
	memset(&Mem::instance().memInfo, 0, sizeof(MemInfo));
	Mem::instance().globalTrackingBlockHead = 0;
	Mem::instance().heapHead = 0;
	Mem::instance().initialized = true;

	return Mem_OK;
}

// TODO get rid of this initialized nonsense for fucks sake, and just have this method free up all the heaps
MemReturnCode Mem::destroy()
{
	Mem& mem = Mem::instance();
	if (!mem.initialized)
		return Mem_ERROR_Mem_Not_Initialized;
	
	while (mem.memInfo.currHeapCount > 0)
		mem.privDestroyHeap(mem.heapHead);
	
	mem.initialized = false;

	return Mem_OK;
}

MemReturnCode Mem::getInfo(MemInfo &retInfo)
{
	Mem& mem = Mem::instance();
	if (!mem.initialized)
		return Mem_ERROR_Mem_Not_Initialized;
	
	memcpy(&retInfo, &mem.memInfo, sizeof(MemInfo));
	return Mem_OK;
}

Heap *Mem::DebugGetHeapHead()
{
	return Mem::instance().heapHead;
}

TrackingBlock *Mem::DebugGetGlobalTrackingHead()
{
	return Mem::instance().globalTrackingBlockHead;
}

MemReturnCode Mem::destroyHeap(Heap* inHeap)
{
	return Mem::instance().privDestroyHeap(inHeap);
}

MemReturnCode Mem::getHeapByAddr(Heap*& pHeap, const void* p)
{
	Mem& mem = Mem::instance();
	if (!mem.initialized)
		return Mem_ERROR_Mem_Not_Initialized;

	unsigned int pInt = (unsigned int) p;
	pHeap = mem.heapHead;
	while (pHeap != 0 && (pInt < pHeap->heapInfo.heapStartAddr || pInt >= pHeap->heapInfo.heapEndAddr))
		pHeap = pHeap->nextHeap;

	if (pHeap == 0)
		return Mem_ERROR_Invalid_Addr;
	else
		return Mem_OK;
}





// --------------------------------------------------------------
// Private method implementations
// --------------------------------------------------------------
Mem::Mem() :
	heapHead(0),
	globalTrackingBlockHead(0),
	memInfo(),
	initialized(false)
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

MemReturnCode Mem::privCreateHeap(Heap *&newHeap, int heapSize, const char * const heapName)
{
	assert(heapSize > 0);
	assert(heapName != 0);

	if (!this->initialized)
		return Mem_ERROR_Mem_Not_Initialized;

	HANDLE hHeap = HeapCreate(0, heapSize, 0);
	if (hHeap == 0)
		return Mem_ERROR_Heap_Create;

	newHeap = (Heap*) HeapAlloc(hHeap, 0, sizeof(VariableBlockHeap));
	newHeap = (Heap*) new(newHeap) VariableBlockHeap(heapSize, heapName, hHeap, this);
	
	if (this->heapHead != 0)
		this->heapHead->prevHeap = newHeap;
	
	newHeap->nextHeap = this->heapHead;
	this->heapHead = newHeap;
	
	this->memInfo.currHeapCount++;
	this->memInfo.peakHeapCount = max(this->memInfo.peakHeapCount, this->memInfo.currHeapCount);

	return Mem_OK;
}

MemReturnCode Mem::privCreateFixedBlockHeap(Heap*& newHeap, int numBlocks, int sizePerBlock, const char * const heapName)
{
	assert(numBlocks > 0);
	assert(sizePerBlock > 0);
	assert(heapName != 0);

	if (!this->initialized)
		return Mem_ERROR_Mem_Not_Initialized;

	HANDLE hHeap = HeapCreate(0, numBlocks * sizePerBlock + sizeof(Heap), 0);
	if (hHeap == 0)
		return Mem_ERROR_Heap_Create;

	newHeap = (Heap*) HeapAlloc(hHeap, 0, sizeof(FixedBlockHeap));
	newHeap = (Heap*) new(newHeap) FixedBlockHeap(numBlocks, sizePerBlock, heapName, hHeap, this);

	if (this->heapHead != 0)
		this->heapHead->prevHeap = newHeap;
	
	newHeap->nextHeap = this->heapHead;
	this->heapHead = newHeap;

	this->memInfo.currHeapCount++;
	this->memInfo.peakHeapCount = max(this->memInfo.peakHeapCount, this->memInfo.currHeapCount);

	return Mem_OK;
}

MemReturnCode Mem::privDestroyHeap(Heap* inHeap)
{
	assert(inHeap != 0);

	if (!this->initialized)
		return Mem_ERROR_Mem_Not_Initialized;

// TODO uncomment out directive after milestone 1
//#ifdef _DEBUG
	TrackingBlock* block = inHeap->DebugGetHeapTrackingHead();
	
	while (block != 0)
	{
		if (block->gNext != 0)
			block->gNext->gPrev = block->gPrev;
		
		if (block->gPrev != 0)
			block->gPrev->gNext = block->gNext;

		block = block->hNext;
	}
//#endif

	this->memInfo.currNumAlloc -= inHeap->heapInfo.currNumAlloc;
	this->memInfo.currBytesUsed -= inHeap->heapInfo.currBytesUsed;
	this->memInfo.currHeapCount--;
	
	if (inHeap->nextHeap != 0)
		inHeap->nextHeap->prevHeap = inHeap->prevHeap;
	
	if (inHeap->prevHeap != 0)
		inHeap->prevHeap->nextHeap = inHeap->nextHeap;

	if (this->heapHead == inHeap)
		this->heapHead = inHeap->nextHeap;

	HeapDestroy(inHeap->winHeapHandle);

	return Mem_OK;
}
