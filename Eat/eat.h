#pragma once

#include "ChunkHeader.h"

bool eat(const char * const archiveName, ChunkType type, const char * const chunkName, unsigned char *&chunkBuff, int &chunkSize);