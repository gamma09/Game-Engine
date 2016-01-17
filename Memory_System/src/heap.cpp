#include <GameAssert.h>
#include <new.h>  // for placement new
#include <math.h>

#include "heap.h"
#include "mem.h"
#include "block.h"


unsigned int get_alignment(Align);

// --------------------------------------------------------------
// HeapInfo implementation
// --------------------------------------------------------------
HeapInfo::HeapInfo()
{
	// Do nothing
}

HeapInfo::HeapInfo(unsigned int size, const char* const _name, unsigned int startAddr) :
	heapStartAddr(startAddr),
	heapEndAddr(startAddr + size),
	totalHeapSize(size),
	peakNumAlloc(0),
	currNumAlloc(0),
	peakBytesUsed(0),
	currBytesUsed(0)
{
	strncpy_s(this->heapName, _name, _TRUNCATE);
}

HeapInfo::HeapInfo(const HeapInfo& info)
{
	memcpy(this, &info, sizeof(HeapInfo));
}

HeapInfo& HeapInfo::operator=(const HeapInfo& info)
{
	memcpy(this, &info, sizeof(HeapInfo));

	return *this;
}

HeapInfo::~HeapInfo()
{
	// Do nothing
}


// --------------------------------------------------------------
// Heap implementation
// --------------------------------------------------------------

Heap::Heap(HANDLE win32Heap, unsigned int size, const char* const name, Mem* _mem) :
	winHeapHandle(win32Heap),
	nextHeap(0),
	prevHeap(0),
	mem(_mem),
	heapInfo(size, name, (unsigned int) this)
{
}

void Heap::getInfo(HeapInfo &retInfo) const
{
	memcpy(&retInfo, &this->heapInfo, sizeof(HeapInfo));
}

Heap* Heap::getNextHeap() const
{
	return this->nextHeap;
}

Heap* Heap::getPrevHeap() const
{
	return this->prevHeap;
}





// --------------------------------------------------------------
// VariableBlockHeap implementation
// --------------------------------------------------------------
VariableBlockHeap::VariableBlockHeap(unsigned int _size, const char* const _name, HANDLE hWin32Heap, Mem* _mem) :
	Heap(hWin32Heap, _size, _name, _mem),
	trackingBlockHead(0)
{
}

void* VariableBlockHeap::alloc(size_t size, Align align, const char* const name, int lineNum)
{
	unsigned int alignment = get_alignment(align);
	unsigned int alignedSize = size + sizeof(TrackingBlock) + alignment - 1;

	void* ptr = HeapAlloc(this->winHeapHandle, 0, alignedSize);
	TrackingBlock* block = new(ptr) TrackingBlock(name, lineNum);
	block->allocIndex = ++this->mem->memInfo.currAllocIndex;
	block->allocSize = alignedSize;

	block->gNext = this->mem->globalTrackingBlockHead;
	if (this->mem->globalTrackingBlockHead != 0)
		this->mem->globalTrackingBlockHead->gPrev = block;
	this->mem->globalTrackingBlockHead = block;

	block->hNext = this->trackingBlockHead;
	if (this->trackingBlockHead != 0)
		this->trackingBlockHead->hPrev = block;
	this->trackingBlockHead = block;
	
	this->heapInfo.currNumAlloc++;
	this->heapInfo.peakNumAlloc = max(this->heapInfo.currNumAlloc, this->heapInfo.peakNumAlloc);

	this->heapInfo.currBytesUsed += alignedSize;
	this->heapInfo.peakBytesUsed = max(this->heapInfo.currBytesUsed, this->heapInfo.peakBytesUsed);

	// Have we exceeded the amount of memory specified for heap size?
	GameAssert((this->heapInfo.totalHeapSize - sizeof(VariableBlockHeap)) >= this->heapInfo.currBytesUsed);
	
	this->mem->memInfo.currBytesUsed += alignedSize;
	this->mem->memInfo.peakBytesUsed = max(this->mem->memInfo.currBytesUsed, this->mem->memInfo.peakBytesUsed);

	this->mem->memInfo.currNumAlloc++;
	this->mem->memInfo.peakNumAlloc = max(this->mem->memInfo.peakNumAlloc, this->mem->memInfo.currNumAlloc);

	void* p = block + 1;

	void* alignedAddr = (void*) (((unsigned int) p + alignment - 1) & ~(alignment - 1));
	TrackingBlock** topPtr = (TrackingBlock**) alignedAddr - 1;
	*topPtr = block;

	return alignedAddr;
}

void VariableBlockHeap::free(void* p)
{
	TrackingBlock* block = *((TrackingBlock**) p - 1);

	if (block->gNext != 0)
		block->gNext->gPrev = block->gPrev;
	if (block->gPrev != 0)
		block->gPrev->gNext = block->gNext;

	if (block->hNext != 0)
		block->hNext->hPrev = block->hPrev;
	if (block->hPrev != 0)
		block->hPrev->hNext = block->hNext;

	this->heapInfo.currBytesUsed -= block->allocSize;
	this->heapInfo.currNumAlloc--;

	this->mem->memInfo.currBytesUsed -= block->allocSize;
	this->mem->memInfo.currNumAlloc--;

	if (this->trackingBlockHead == block)
		this->trackingBlockHead = block->hNext;

	if (this->mem->globalTrackingBlockHead == block)
		this->mem->globalTrackingBlockHead = block->gNext;

	HeapFree(this->winHeapHandle, 0, block);
}

TrackingBlock* VariableBlockHeap::DebugGetHeapTrackingHead() const
{
	return this->trackingBlockHead;
}



// --------------------------------------------------------------
// FixedBlockHeap implementation
// --------------------------------------------------------------
FixedBlockHeap::FixedBlockHeap(unsigned int numBlocks, unsigned int _blockSize, const char* const name, HANDLE hWin32Heap, Mem* _mem) :
	Heap(hWin32Heap, numBlocks * max((int) ceil(_blockSize / 4.0f) * 4, sizeof(FreeBlock)), name, _mem),
	blockSize(max((int) ceil(_blockSize / 4.0f) * 4, sizeof(FreeBlock)))
{
	this->blocks = HeapAlloc(hWin32Heap, 0, this->blockSize * numBlocks + 3);
	this->alignedFreeHead = (FreeBlock*) (((unsigned int) this->blocks + 3) & ~3);
	FreeBlock* curr = this->alignedFreeHead;

	for (unsigned int i = 0; i < numBlocks - 1; i++) {
		curr->next = (FreeBlock*) ((unsigned int) curr + this->blockSize);
		curr = curr->next;
	}

	curr->next = 0;
}

void* FixedBlockHeap::alloc(size_t size, Align align, const char* const, int)
{
	GameAssert(size <= this->blockSize);
	GameAssert(align == ALIGN_4);

	align;
	size;

	// Are we out of blocks?
	GameAssert(this->alignedFreeHead != 0);

	void* p = this->alignedFreeHead;
	this->alignedFreeHead = this->alignedFreeHead->next;

	this->heapInfo.currBytesUsed += this->blockSize;
	this->heapInfo.peakBytesUsed = max(this->heapInfo.currBytesUsed, this->heapInfo.peakBytesUsed);

	this->heapInfo.currNumAlloc++;
	this->heapInfo.peakNumAlloc = max(this->heapInfo.currNumAlloc, this->heapInfo.peakNumAlloc);

	this->mem->memInfo.currBytesUsed += this->blockSize;
	this->mem->memInfo.peakBytesUsed = max(this->mem->memInfo.currBytesUsed, this->mem->memInfo.peakBytesUsed);

	this->mem->memInfo.currNumAlloc++;
	this->mem->memInfo.peakNumAlloc = max(this->mem->memInfo.currNumAlloc, this->mem->memInfo.peakNumAlloc);

	this->mem->memInfo.currAllocIndex++;

	return p;
}

void FixedBlockHeap::free(void* p)
{
	FreeBlock* block = (FreeBlock*) p;
	block->next = this->alignedFreeHead;
	this->alignedFreeHead = block;

	this->heapInfo.currBytesUsed -= this->blockSize;
	this->heapInfo.currNumAlloc--;

	this->mem->memInfo.currBytesUsed -= this->blockSize;
	this->mem->memInfo.currNumAlloc--;
}

TrackingBlock* FixedBlockHeap::DebugGetHeapTrackingHead() const
{
	return 0;
}


unsigned int inline get_alignment(Align align) {
	switch (align) {
	case ALIGN_4:
		return 4;

	case ALIGN_8:
		return 8;

	case ALIGN_16:
		return 16;

	case ALIGN_32:
		return 32;

	case ALIGN_64:
		return 64;

	case ALIGN_128:
		return 128;

	case ALIGN_256:
		return 256;

	default:
		return 1;
	}
}