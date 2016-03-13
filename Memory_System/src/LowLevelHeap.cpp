#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <Windows.h>
#include <malloc.h>
#include <utility>
#include <GameAssert.h>
#include "LowLevelHeap.h"

static unsigned int IncreaseToFullPageSize( unsigned int heapSize )
{
	SYSTEM_INFO info;
	GetSystemInfo( &info );
	unsigned int remainingBytes = info.dwPageSize - heapSize % info.dwPageSize;
	GameAssert( ( heapSize + remainingBytes ) % info.dwPageSize == 0 );
	return heapSize + remainingBytes;
}

static_assert( sizeof( LowLevelFreeBlock ) == sizeof( LowLevelUsedBlock ), "Free block and used block are not the same size." );

LowLevelHeap::LowLevelHeap( unsigned int heapSize )
	: size( IncreaseToFullPageSize( heapSize ) ),
	usedHead( nullptr ),
	usedTail( nullptr )
{
	this->rawMemory = malloc( this->size );

	this->freeHead = ( LowLevelFreeBlock* ) this->rawMemory;
	this->freeHead->size = this->size - sizeof( LowLevelFreeBlock );
	this->freeHead->next = nullptr;
	this->freeHead->prev = nullptr;

	this->freeTail = this->freeHead;
}

LowLevelHeap::LowLevelHeap( LowLevelHeap&& heap )
{
	*this = std::move( heap );
}

LowLevelHeap& LowLevelHeap::operator=( LowLevelHeap&& heap )
{
	this->rawMemory = heap.rawMemory;
	this->usedHead = heap.usedHead;
	this->usedTail = heap.usedTail;
	this->freeHead = heap.freeHead;
	this->freeTail = heap.freeTail;

	memset( &heap, 0, sizeof( LowLevelHeap ) );
	return *this;
}

LowLevelHeap::~LowLevelHeap()
{
	free( this->rawMemory );
}

static LowLevelUsedBlock* FindNextUsedBlock( LowLevelFreeBlock* freeBlock, LowLevelUsedBlock* heapTail )
{
	if( heapTail == nullptr )
	{
		return nullptr;
	}

	GameAssert( freeBlock != nullptr );

	static_assert( sizeof( unsigned char ) == 1, "unsigned char is not 1 byte wide" );
	LowLevelUsedBlock* nextUsed = (LowLevelUsedBlock*) ( ( (unsigned char*) freeBlock ) + freeBlock->size );
	if( nextUsed > heapTail )
	{
		// This free block is the last block in the heap
		return nullptr;
	}
	else
	{
		return nextUsed;
	}
}

void* LowLevelHeap::Allocate( unsigned int inSize )
{
	LowLevelFreeBlock* current = this->freeHead;
	while( current != nullptr && current->size < inSize )
	{
		current = current->next;
	}

	GameCheckFatal( current != nullptr, "Out of memory." );

	LowLevelFreeBlock* nextFree = current->next;
	LowLevelFreeBlock* prevFree = current->prev;
	LowLevelUsedBlock* nextUsed = FindNextUsedBlock( current, this->usedTail );
	LowLevelUsedBlock* prevUsed = (prevFree == nullptr ) ? ((size_t) this->usedHead > (size_t) current ? nullptr : this->usedHead) : FindNextUsedBlock( prevFree, this->usedTail );

	if( inSize + sizeof( LowLevelFreeBlock ) <= current->size )
	{
		// This block is large enough for us to split
		SplitBlock( current, nextFree, inSize );
		
	}
	
	AllocateFullBlock( current, prevFree, nextFree, prevUsed, nextUsed );

	return current + 1;
}

void LowLevelHeap::AllocateFullBlock( LowLevelFreeBlock* freeBlock, LowLevelFreeBlock* prevFree, LowLevelFreeBlock* nextFree, LowLevelUsedBlock* prevUsed, LowLevelUsedBlock* nextUsed )
{
	LowLevelUsedBlock* currentUsed = (LowLevelUsedBlock*) freeBlock;
	currentUsed->prev = prevUsed;
	currentUsed->next = nextUsed;
	if( nextUsed != nullptr )
	{
		nextUsed->prev = currentUsed;
	}
	else
	{
		this->usedTail = currentUsed;
	}

	if( prevUsed != nullptr )
	{
		prevUsed->next = currentUsed;
	}
	else
	{
		this->usedHead = currentUsed;
	}

	if( prevFree != nullptr )
	{
		prevFree->next = nextFree;
	}
	else
	{
		this->freeHead = nextFree;
	}

	if( nextFree != nullptr )
	{
		nextFree->prev = prevFree;
	}
	else
	{
		this->freeTail = prevFree;
	}
}

void LowLevelHeap::SplitBlock( LowLevelFreeBlock* blockToSplit, LowLevelFreeBlock*& nextFree, unsigned int inSize )
{
	LowLevelFreeBlock* newNextFree = (LowLevelFreeBlock*) ( (unsigned char*) ( blockToSplit + 1 ) + inSize );
	newNextFree->next = nextFree;
	newNextFree->prev = blockToSplit;
	if( nextFree != nullptr )
	{
		nextFree->prev = newNextFree;
	}
	else
	{
		this->freeTail = blockToSplit->prev;
	}

	blockToSplit->next = newNextFree;
	nextFree = newNextFree;

	newNextFree->size = blockToSplit->size - sizeof( LowLevelFreeBlock ) - inSize;
	blockToSplit->size = inSize;
}

static void GetSurroundingFreeBlocks( LowLevelUsedBlock* usedBlock, LowLevelFreeBlock* freeHead, LowLevelFreeBlock* freeTail, LowLevelFreeBlock*& prevFree, LowLevelFreeBlock*& nextFree )
{
	if( freeHead == nullptr || (size_t) freeHead > (size_t) usedBlock )
	{
		prevFree = nullptr;
		nextFree = freeHead;
		return;
	}

	LowLevelUsedBlock* current = usedBlock;
	while( current->next != nullptr && (size_t) (current + 1) + current->size == (size_t) current->next )
	{
		current = current->next;
	}

	nextFree = (LowLevelFreeBlock*) ( (size_t) ( current + 1 ) + current->size );
	if( nextFree > freeTail )
	{
		nextFree = nullptr;
		prevFree = freeTail;
	}
	else
	{
		prevFree = nextFree->prev;
	}
}

void LowLevelHeap::Free( void* ptr )
{
	GameAssert( ptr != nullptr );

	LowLevelUsedBlock* current = (LowLevelUsedBlock*) ptr - 1;
	LowLevelFreeBlock* prevFree;
	LowLevelFreeBlock* nextFree;
	GetSurroundingFreeBlocks( current, this->freeHead, this->freeTail, prevFree, nextFree );

	if( current->prev == nullptr )
	{
		this->usedHead = current->next;
	}
	else
	{
		current->prev->next = current->next;
	}

	if( current->next == nullptr )
	{
		this->usedTail = current->prev;
	}
	else
	{
		current->next->prev = current->prev;
	}

	LowLevelFreeBlock* currentFree = (LowLevelFreeBlock*) current;
	currentFree->prev = prevFree;
	currentFree->next = nextFree;

	if( currentFree->prev == nullptr )
	{
		this->freeHead = currentFree;
	}
	else
	{
		currentFree->prev->next = currentFree;
	}

	if( currentFree->next == nullptr )
	{
		this->freeTail = currentFree;
	}
	else
	{
		currentFree->next->prev = currentFree;
	}

	if( BlocksAreAdjacent( currentFree, currentFree->next ) )
	{
		MergeBlocks( currentFree, currentFree->next );
	}

	if( BlocksAreAdjacent( currentFree->prev, currentFree ) )
	{
		MergeBlocks( currentFree->prev, currentFree );
	}
}

bool LowLevelHeap::BlocksAreAdjacent( const LowLevelFreeBlock* firstBlock, const LowLevelFreeBlock* secondBlock ) const
{
	if( firstBlock == nullptr || secondBlock == nullptr ) return false;

	return (size_t) ( firstBlock + 1 ) + firstBlock->size == (size_t) secondBlock;
}

void LowLevelHeap::MergeBlocks( LowLevelFreeBlock* firstBlock, LowLevelFreeBlock* secondBlock )
{
	firstBlock->size += secondBlock->size + sizeof( LowLevelFreeBlock );

	firstBlock->next = secondBlock->next;
	if( firstBlock->next != nullptr )
	{
		firstBlock->next->prev = firstBlock;
	}
	else
	{
		this->freeTail = firstBlock;
	}
}