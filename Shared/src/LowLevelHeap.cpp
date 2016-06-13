#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <Windows.h>
#include <malloc.h>
#include <utility>
#include "LowLevelHeap.h"
#include "GameAssert.h"

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

#ifdef _DEBUG
	memset( this->freeHead->prepadding, 0xEE, BLOCK_PADDING_SIZE );
	memset( this->freeHead->postpadding, 0xEE, BLOCK_PADDING_SIZE );
	this->freeHead->visited = false;
#endif

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
	this->size = heap.size;

	memset( &heap, 0, sizeof( LowLevelHeap ) );
	return *this;
}

LowLevelHeap::~LowLevelHeap()
{
	if( this->rawMemory != nullptr )
	{
		free( this->rawMemory );
	}
}

#ifdef _DEBUG

unsigned int LowLevelHeap::CountAllocations() const
{
	LowLevelUsedBlock* curr = this->usedHead;
	unsigned int count = 0;
	while( curr != nullptr )
	{
		count++;
		curr = curr->next;
	}

	return count;
}

static void ValidateUsedList( LowLevelUsedBlock* head, LowLevelUsedBlock* tail )
{
	LowLevelUsedBlock* prev = head;
	LowLevelUsedBlock* curr = prev == nullptr ? nullptr : prev->next;
	if( head != nullptr )
	{
		// If the head is not null, we have at least 1 block
		GameAssert( tail != nullptr );

		// Make sure the head really is the first one in the list
		GameAssert( head->prev == nullptr );

		// Make sure the tail really is the last one in the list
		GameAssert( tail->next == nullptr );

		// Make sure the validity checker is not in some weird state
		GameAssert( !head->visited );
		GameAssert( !tail->visited );

		for( int i = 0; i < BLOCK_PADDING_SIZE; i++ )
		{
			GameAssert( head->prepadding[i] == 0xEE );
			GameAssert( head->postpadding[i] == 0xEE );
		}

		head->visited = true;
	}

	while( curr != nullptr )
	{
		// Make sure curr and prev are doubly-linked
		GameAssert( curr->prev == prev );

		// We want to make sure we don't have a circular list
		GameAssert( !curr->visited );

		// Make sure the list is in address order
		GameAssert( prev < curr );

		for( int i = 0; i < BLOCK_PADDING_SIZE; i++ )
		{
			GameAssert( curr->prepadding[i] == 0xEE );
			GameAssert( curr->postpadding[i] == 0xEE );
		}

		curr->visited = true;
		curr = curr->next;
		prev = prev->next;
	}

	// Make sure tail matches - if head is nullptr, then prev will also be a nullptr at this point
	GameAssert( prev == tail );
}

static void ValidateFreeList( LowLevelFreeBlock* head, LowLevelFreeBlock* tail )
{
	LowLevelFreeBlock* prev = head;
	LowLevelFreeBlock* curr = prev == nullptr ? nullptr : prev->next;
	if( head != nullptr )
	{
		// If the head is not null, we have at least 1 block
		GameAssert( tail != nullptr );

		// Make sure the head really is the first one in the list
		GameAssert( head->prev == nullptr );

		// Make sure the tail really is the last one in the list
		GameAssert( tail->next == nullptr );

		// Make sure the validity checker is not in some weird state
		GameAssert( !head->visited );
		GameAssert( !tail->visited );

		for( int i = 0; i < BLOCK_PADDING_SIZE; i++ )
		{
			GameAssert( head->prepadding[i] == 0xEE );
			GameAssert( head->postpadding[i] == 0xEE );
		}

		head->visited = true;
	}

	while( curr != nullptr )
	{
		// Make sure curr and prev are doubly-linked
		GameAssert( curr->prev == prev );

		// We want to make sure we don't have a circular list
		GameAssert( !curr->visited );

		// Make sure the list is in address order
		GameAssert( prev < curr );

		for( int i = 0; i < BLOCK_PADDING_SIZE; i++ )
		{
			GameAssert( curr->prepadding[i] == 0xEE );
			GameAssert( curr->postpadding[i] == 0xEE );
		}

		curr->visited = true;
		curr = curr->next;
		prev = prev->next;
	}

	// Make sure tail matches - if head is nullptr, then prev will also be a nullptr at this point
	GameAssert( prev == tail );
}

bool EqualsWithinTolerance( unsigned int a, unsigned int b, unsigned int tolerance )
{
	long long sa = (long long) a;
	long long sb = (long long) b;

	long long diff = sa - sb;
	if( diff < 0 )
	{
		diff *= -1;
	}

	return diff < tolerance;
}

void LowLevelHeap::CheckValidity() const
{
	// First, make sure the used blocks list is valid
	ValidateUsedList( this->usedHead, this->usedTail );

	// Then, make sure the free blocks list is valid - this also makes sure that the used list and the free list
	// are mutually exclusive
	ValidateFreeList( this->freeHead, this->freeTail );

	// Make sure the head and tail of each list is within the raw memory area
	GameAssert( ( size_t ) this->usedHead >= ( size_t ) this->rawMemory );
	GameAssert( ( size_t ) this->freeHead >= ( size_t ) this->rawMemory );

	GameAssert( ( size_t ) this->usedTail < ( size_t ) this->rawMemory + this->size );
	GameAssert( ( size_t ) this->freeTail < ( size_t ) this->rawMemory + this->size );

	// Go through both lists and make sure we don't have elements in both
	for( LowLevelUsedBlock* currUsed = this->usedHead; currUsed; currUsed = currUsed->next )
	{
		for( LowLevelFreeBlock* currFree = this->freeHead; currFree; currFree = currFree->next )
		{
			GameAssert( currUsed != reinterpret_cast<LowLevelUsedBlock*>( currFree ) );
		}
	}

	// Finally, go back through both lists, tally up the total size, and make sure that it is reasonable
	// ...also, reset all of the visited booleans back to false for next time
	size_t allocationsSize = 0;
	for( LowLevelUsedBlock* curr = this->usedHead; curr != nullptr; curr = curr->next )
	{
		allocationsSize += sizeof( LowLevelUsedBlock ) + curr->size;
		curr->visited = false;
	}

	for( LowLevelFreeBlock* curr = this->freeHead; curr != nullptr; curr = curr->next )
	{
		allocationsSize += sizeof( LowLevelFreeBlock ) + curr->size;
		curr->visited = false;
	}

	// Make sure the total allocation and free size is within an order of magnitude of the heap size
	GameAssert( EqualsWithinTolerance( allocationsSize, this->size, this->size / 10 ) );
}

bool LowLevelHeap::ContainsUsedBlock( LowLevelUsedBlock* block ) const
{
	for( LowLevelUsedBlock* curr = this->usedHead; curr; curr = curr->next )
	{
		if( curr == block )
		{
			return true;
		}
	}

	return false;
}

#endif

static LowLevelUsedBlock* FindNextUsedBlock( LowLevelFreeBlock* freeBlock, LowLevelUsedBlock* heapTail )
{
	if( heapTail == nullptr )
	{
		return nullptr;
	}

	GameAssert( freeBlock != nullptr );

	static_assert( sizeof( unsigned char ) == 1, "unsigned char is not 1 byte wide" );
	LowLevelUsedBlock* nextUsed = (LowLevelUsedBlock*) ( ( (unsigned char*) freeBlock ) + freeBlock->size + sizeof( LowLevelFreeBlock ) );
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
	LowLevelUsedBlock* prevUsed = ( nextUsed == nullptr ? this->usedTail : nextUsed->prev );

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

#ifdef _DEBUG
	currentUsed->visited = false;
#endif
}

void LowLevelHeap::SplitBlock( LowLevelFreeBlock* blockToSplit, LowLevelFreeBlock*& nextFree, unsigned int inSize )
{
	LowLevelFreeBlock* newNextFree = (LowLevelFreeBlock*) ( (unsigned char*) ( blockToSplit + 1 ) + inSize );

#ifdef _DEBUG
	newNextFree->visited = false;
	memset( newNextFree->prepadding, 0xEE, BLOCK_PADDING_SIZE );
	memset( newNextFree->postpadding, 0xEE, BLOCK_PADDING_SIZE );
#endif

	newNextFree->next = nextFree;
	newNextFree->prev = blockToSplit;
	if( nextFree != nullptr )
	{
		nextFree->prev = newNextFree;
	}
	else
	{
		this->freeTail = newNextFree;
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
	while( current->next != nullptr && (size_t) ( current + 1 ) + current->size == (size_t) current->next )
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

	GameAssert( this->ContainsUsedBlock( current ) );
	
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