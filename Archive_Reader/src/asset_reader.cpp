#include "asset_reader.h"

#include <File.h>
#include <cstring>
#include <GameAssert.h>
#include <MemoryEngine.h>
#include "PackageHeader.h"

bool read_asset( const char * const archiveFilename, ChunkType type, const char * const chunkName, unsigned char*& chunkBuff, int& chunkSize, Heap* heap )
{
	FileHandle archive;
	FileError status = File::open( archive, archiveFilename, FileMode::FILE_READ );

	if( status != FILE_SUCCESS )
	{
		DWORD errorCode = GetLastError();
		errorCode;
		return false;
	}

	PackageHeader header;
	GameVerify( FILE_SUCCESS == File::read( archive, &header, sizeof( PackageHeader ) ) );

	int chunksRemaining = header.numChunks;

	while( chunksRemaining > 0 )
	{
		ChunkHeader chunkHdr;
		GameVerify( FILE_SUCCESS == File::read( archive, &chunkHdr, sizeof( ChunkHeader ) ) );
		if( chunkHdr.type == type && 0 == strcmp( chunkName, chunkHdr.chunkName ) )
		{
			chunkSize = chunkHdr.chunkSize;
			chunkBuff = newArray( unsigned char, chunkSize, heap, ALIGN_4);
			GameVerify( FILE_SUCCESS == File::read( archive, chunkBuff, chunkSize ) );

			File::close( archive );
			return true;
		}
		else
		{
			GameVerify( FILE_SUCCESS == File::seek( archive, FileSeek::FILE_SEEK_CURRENT, chunkHdr.chunkSize ) );
			chunksRemaining--;
		}
	}

	File::close( archive );
	return false;
}