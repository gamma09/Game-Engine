#pragma once

struct LowLevelFreeBlock
{
	unsigned int size;
	LowLevelFreeBlock* next;
	LowLevelFreeBlock* prev;
};

struct LowLevelUsedBlock
{
	unsigned int size;
	LowLevelUsedBlock* next;
	LowLevelUsedBlock* prev;
};

class LowLevelHeap
{
public:
	LowLevelHeap( unsigned int heapSize );
	LowLevelHeap( LowLevelHeap&& heap );
	LowLevelHeap& operator=( LowLevelHeap&& heap );
	~LowLevelHeap();

	void* Allocate( unsigned int size );
	void Free( void* ptr );

private:
	void AllocateFullBlock( LowLevelFreeBlock* freeBlock, LowLevelFreeBlock* prevFree, LowLevelFreeBlock* nextFree, LowLevelUsedBlock* prevUsed, LowLevelUsedBlock* nextUsed );
	void SplitBlock( LowLevelFreeBlock* blockToSplit, LowLevelFreeBlock*& nextFree, unsigned int inSize );
	bool BlocksAreAdjacent( const LowLevelFreeBlock* firstBlock, const LowLevelFreeBlock* secondBlock ) const;
	void MergeBlocks( LowLevelFreeBlock* firstBlock, LowLevelFreeBlock* secondBlock );



	LowLevelHeap( const LowLevelHeap& ) = delete;
	LowLevelHeap& operator=( const LowLevelHeap& ) = delete;

private:
	void* rawMemory;
	LowLevelFreeBlock* freeHead;
	LowLevelFreeBlock* freeTail;
	LowLevelUsedBlock* usedHead;
	LowLevelUsedBlock* usedTail;
	unsigned int size;
};