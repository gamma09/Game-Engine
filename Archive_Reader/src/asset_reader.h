#pragma once

#include "ChunkHeader.h"

class Heap;

bool read_asset( const char * const archiveName, ChunkType type, const char * const chunkName, unsigned char *&chunkBuff, int &chunkSize, Heap* heap );