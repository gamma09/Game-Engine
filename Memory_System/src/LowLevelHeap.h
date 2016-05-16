#pragma once

#ifdef _DEBUG
#define BLOCK_PADDING_SIZE 16
#endif

struct LowLevelFreeBlock
{
#ifdef _DEBUG
	// Used to ensure no one is corrupting the block header memory
	unsigned char prepadding[BLOCK_PADDING_SIZE];
#endif

	unsigned int size;
	LowLevelFreeBlock* next;
	LowLevelFreeBlock* prev;

	
#ifdef _DEBUG
	// Used to ensure the low level heap is not revisiting nodes
	bool visited;

	// Used to ensure no one is corrupting the block header memory
	unsigned char postpadding[BLOCK_PADDING_SIZE];
#endif
};

struct LowLevelUsedBlock
{
#ifdef _DEBUG
	// Used to ensure no one is corrupting the block header memory
	unsigned char prepadding[BLOCK_PADDING_SIZE];
#endif

	unsigned int size;
	LowLevelUsedBlock* next;
	LowLevelUsedBlock* prev;

	
#ifdef _DEBUG
	// Used to ensure the low level heap is not revisiting nodes
	bool visited;
	// Used to ensure no one is corrupting the block header memory
	unsigned char postpadding[BLOCK_PADDING_SIZE];
#endif
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

#ifdef _DEBUG
	unsigned int CountAllocations() const;
	void CheckValidity() const;
	bool ContainsUsedBlock( LowLevelUsedBlock* block ) const;
#endif

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