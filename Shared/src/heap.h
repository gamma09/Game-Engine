#pragma once

#include <utility>
#include "Enums.h"
#include "LowLevelHeap.h"

// forward declaration
class TrackingBlock;

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
	unsigned int currBytesFree;
	char heapName[20];
	bool isSystemHeap;

	HeapInfo();
	HeapInfo( unsigned int size, unsigned int startAddr, const char* name );
	HeapInfo( const HeapInfo& info );
	HeapInfo& operator=( const HeapInfo& rhs );
	~HeapInfo();
};

// Heap class
class Heap
{
public:
	virtual ~Heap();

	inline const HeapInfo& getInfo() const { return this->heapInfo; }

	// -----------------------------------------------------------------------------------
	// Add extra data or methods below this line
	// -----------------------------------------------------------------------------------

	virtual void* alloc( size_t size, Align align, const char* name, int lineNum ) abstract;
	virtual void free( void* p ) abstract;
	virtual const char* GetHeapTypeString() const abstract;

	virtual TrackingBlock* GetHeapTrackingHead() const { return nullptr; }
	inline LowLevelHeap&& RipOutLowLevelHeap() { return std::move( lowLevelHeap ); }

#ifdef _DEBUG
	inline const LowLevelHeap* GetLowLevelHeap() { return &this->lowLevelHeap; }
	void DebugPrintHeapLeaks() const;
#endif

protected:
	Heap( LowLevelHeap&& lowLevelHeap, unsigned int size, Mem* mem, const char* heapName );

protected:
	// data -----------------------------------------------------------------------
	// Links to the next heaps
	Heap*    nextHeap;
	Heap*    prevHeap;
	LowLevelHeap lowLevelHeap;
	HeapInfo heapInfo;

	// Back link to the memory system (You may not need this field)
	Mem     *mem;

	friend class Mem;
};

class VariableBlockHeap : protected Heap
{
public:

	VariableBlockHeap( unsigned int size, LowLevelHeap&& lowLevelHeap, Mem* mem, const char* heapName );
	virtual ~VariableBlockHeap() {}

	virtual void* alloc( size_t size, Align align, const char* name, int lineNum ) override;
	virtual void free( void* p ) override;
	virtual const char* GetHeapTypeString() const override;

	virtual TrackingBlock* GetHeapTrackingHead() const override;

private:
	TrackingBlock     *trackingBlockHead;
};

struct FreeBlock
{
	FreeBlock* next;
};

class FixedBlockHeap : protected Heap
{
public:

	FixedBlockHeap( unsigned int numBlocks, unsigned int blockSize, LowLevelHeap&& lowLevelHeap, Mem* mem, const char* heapName );
	virtual ~FixedBlockHeap() {}

	virtual void* alloc( size_t size, Align align, const char* name, int lineNum ) override;
	virtual void free( void* p ) override;
	virtual const char* GetHeapTypeString() const override;

private:
	unsigned int blockSize;
	void* blocks;
	FreeBlock* alignedFreeHead;
};
