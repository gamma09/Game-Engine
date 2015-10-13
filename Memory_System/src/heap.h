#ifndef HEAP_H
#define HEAP_H

#include <windows.h> // win32
#include "Enums.h"

// forward declaration
// TODO uncomment these directives after milestone 1
//#ifdef _DEBUG
class TrackingBlock;
//#endif
class Mem;

// defines for the heap name length
// TODO refactor these defines away after milestone 1
#define HeapNameLength 32
#define HeapNameNumChar (HeapNameLength-1)

// heap info structure
struct HeapInfo
{
// TODO refactor heap name away after milestone 1
	char heapName[HeapNameLength];
	unsigned int heapStartAddr;
	unsigned int heapEndAddr;
	unsigned int totalHeapSize;
	unsigned int peakNumAlloc;
	unsigned int currNumAlloc;
	unsigned int peakBytesUsed;
	unsigned int currBytesUsed;

	HeapInfo();
	HeapInfo(unsigned int size, const char* const _name, unsigned int startAddr);
	HeapInfo(const HeapInfo& info);
	HeapInfo& operator=(const HeapInfo& rhs);
	~HeapInfo();
};

// TODO Refactor this enum away after milestone 1
// types of heap
enum HeapTypeEnum
{
	HeapType_Normal,
	HeapType_Fixed,
	HeapType_Dword = 0x7fffffff
};

// TODO Refactor to multiple classes after milestone 1
// Refactored class tree:
// Heap
// - FixedBlockHeap
// - VariableBlockHeap

// Heap class
class Heap
{
public:
	// Public Interface ------------------------------   for the customers
  
	void getInfo(HeapInfo &info) const;

	// getTrackingBlockHead
	// TODO remove this blockhead after milestone 1 completely
	virtual TrackingBlock* DebugGetHeapTrackingHead() const abstract;

// TODO uncomment this directive after milestone 1
//#ifdef _DEBUG
	// Hidden -----------------  only accessible for debug mode 

	// Data inside the heap Header
	// TODO should basically never be accessed ever...
	Heap* getNextHeap() const;
	Heap* getPrevHeap() const;
	
// TODO uncomment this directive after milestone 1
//#endif

// -----------------------------------------------------------------------------------
// Add extra data or methods below this line
// -----------------------------------------------------------------------------------

	virtual void* alloc(size_t size, Align align, const char* const name, int lineNum) abstract;
	virtual void free(void* p) abstract;

protected:
	Heap(HANDLE win32Heap, unsigned int size, const char* const name, Mem* mem);

protected:
	// data -----------------------------------------------------------------------
	// Links to the next heaps
	Heap*    nextHeap;
	Heap*    prevHeap;
	HANDLE   winHeapHandle;
	HeapInfo heapInfo;
	
	// Back link to the memory system (You may not need this field)
	Mem     *mem;

	friend class Mem;
};

class VariableBlockHeap : protected Heap
{
public:

	VariableBlockHeap(unsigned int size, const char* const name, HANDLE hWin32Heap, Mem* mem);

	virtual void* alloc(size_t size, Align align, const char* const name, int lineNum);
	virtual void free(void* p);

	// getTrackingBlockHead
	// TODO unvirtualize this after MS1
	virtual TrackingBlock* DebugGetHeapTrackingHead() const;

private:
// TODO uncomment this directive after milestone 1
//#ifdef _DEBUG
	TrackingBlock     *trackingBlockHead;
//#endif
};

struct FreeBlock {
	FreeBlock* next;
};

class FixedBlockHeap : protected Heap
{
public:

	FixedBlockHeap(unsigned int numBlocks, unsigned int blockSize, const char* const name, HANDLE hWin32Heap, Mem* mem);

	// getTrackingBlockHead
	// TODO remove this blockhead completely after milestone 1
	virtual TrackingBlock* DebugGetHeapTrackingHead() const;
	
	virtual void* alloc(size_t size, Align align, const char* const name, int lineNum);
	virtual void free(void* p);

private:
	unsigned int blockSize;
	void* blocks;
	FreeBlock* alignedFreeHead;
};


#endif