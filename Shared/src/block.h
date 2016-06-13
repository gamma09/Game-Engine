#pragma once

// defines for the heap name length
#define TrackingBlockNameLength 32
#define TrackingBlockNameNumChar (HeapNameLength-1)

class TrackingBlock
{
public:
	TrackingBlock( const char* inName, int lineNum );

public:
	// Tracking data -----------------------------------------------------------------
	TrackingBlock* hNext;
	TrackingBlock* hPrev;
	TrackingBlock* gNext;
	TrackingBlock* gPrev;

#ifdef _DEBUG
	char           fileName[TrackingBlockNameLength];
	int            lineNum;
#endif
	int            allocIndex;
	int            allocSize;  // squirrel away the size

	// -----------------------------------------------------------------------------------
	// Add extra data or methods below this line
	// -----------------------------------------------------------------------------------
	TrackingBlock* topPtrPlaceholder; // Reserve 4 bytes for the magic ptr, just in case we dont need to do any alignment correction
};