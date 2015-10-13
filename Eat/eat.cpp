#include "eat.h"

#include <File.h>
#include <cstring>
#include <cassert>

#include "PackageHeader.h"

bool eat(const char * const archiveFilename, ChunkType type, const char * const chunkName, unsigned char*& chunkBuff, int& chunkSize)
{
	FileHandle archive;
	FileError status = File::open(archive, archiveFilename, FileMode::FILE_READ);
	if (status != FILE_SUCCESS)
		return false;

	PackageHeader header;
	status = File::read(archive, &header, sizeof(PackageHeader));
	assert(status == FILE_SUCCESS);

	int chunksRemaining = header.numChunks;

	while (chunksRemaining > 0)
	{
		ChunkHeader chunkHdr;
		status = File::read(archive, &chunkHdr, sizeof(ChunkHeader));
		if (chunkHdr.type == type && 0 == strcmp(chunkName, chunkHdr.chunkName))
		{
			chunkSize = chunkHdr.chunkSize;
			chunkBuff = new unsigned char[chunkSize];
			status = File::read(archive, chunkBuff, chunkSize);
			assert(status == FILE_SUCCESS);

			File::close(archive);
			return true;
		}
		else
		{
			status = File::seek(archive, FileSeek::FILE_SEEK_CURRENT, chunkHdr.chunkSize);
			assert(status == FILE_SUCCESS);
			chunksRemaining--;
		}
	}

	status = File::close(archive);
	return false;
}