#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "block.h"
#include "mem.h"
#include "heap.h"


TrackingBlock::TrackingBlock(const char* inName, int in_lineNum) :
	lineNum(in_lineNum),
	gPrev(0),
	gNext(0),
	hPrev(0),
	hNext(0)
{
	strncpy_s(this->fileName, inName, TrackingBlockNameLength);
}
