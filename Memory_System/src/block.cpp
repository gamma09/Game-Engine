#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "block.h"
#include "mem.h"
#include "heap.h"


TrackingBlock::TrackingBlock(const char* inName, int in_lineNum) :
#ifdef _DEBUG
	lineNum(in_lineNum),
#endif
	gPrev(0),
	gNext(0),
	hPrev(0),
	hNext(0)
{
#ifdef _DEBUG
	strncpy_s(this->fileName, inName, TrackingBlockNameLength);
#else
	inName;
	in_lineNum;
#endif
}
