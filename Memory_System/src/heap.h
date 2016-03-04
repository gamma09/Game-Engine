#pragma once

#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <windows.h> // win32
#include "Enums.h"

// forward declaration
#ifdef _DEBUG
class TrackingBlock;
#endif

class Mem;

// heap info structure
struct HeapInfo
{
	unsigned int heapStartAddr;
	unsigned int heapEndAddr;
	unsigned int totalHeapSize;
	unsigned int peakNumAlloc;
	unsigned int currNumAlloc;
	unsigned int peakBytesUsed;
	unsigned int currBytesUsed;

	HeapInfo();
	HeapInfo( unsigned int size, unsigned int startAddr );
	HeapInfo( const HeapInfo& info );
	HeapInfo& operator=( const HeapInfo& rhs );
	~HeapInfo();
};

// Heap class
class Heap
{
public:
	// Public Interface ------------------------------   for the customers

	inline const HeapInfo& getInfo() const { return this->heapInfo; }

	// -----------------------------------------------------------------------------------
	// Add extra data or methods below this line
	// -----------------------------------------------------------------------------------

	virtual void* alloc( size_t size, Align align, const char* name, int lineNum ) abstract;
	virtual void free( void* p ) abstract;

#ifdef _DEBUG
	virtual TrackingBlock* DebugGetHeapTrackingHead() const { return nullptr; }
#endif

protected:
	Heap( HANDLE win32Heap, unsigned int size, Mem* mem );

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

	VariableBlockHeap( unsigned int size, HANDLE hWin32Heap, Mem* mem );

	virtual void* alloc( size_t size, Align align, const char* name, int lineNum ) override;
	virtual void free( void* p ) override;

	// getTrackingBlockHead
	virtual TrackingBlock* DebugGetHeapTrackingHead() const override;

private:
#ifdef _DEBUG
	TrackingBlock     *trackingBlockHead;
#endif
};

struct FreeBlock
{
	FreeBlock* next;
};

class FixedBlockHeap : protected Heap
{
public:

	FixedBlockHeap( unsigned int numBlocks, unsigned int blockSize, HANDLE hWin32Heap, Mem* mem );

	virtual void* alloc( size_t size, Align align, const char* name, int lineNum ) override;
	virtual void free( void* p ) override;

private:
	unsigned int blockSize;
	void* blocks;
	FreeBlock* alignedFreeHead;
};
