#include <GameAssert.h>
#include <File.h>

#include "FileLoader.h"
#include "MemorySetup.h"

char* FileLoader::Load_File( const char* fileName, unsigned long& size )
{
	GameAssert( fileName != 0 );

	FileHandle file;
	FileError status;

	status = File::open( file, fileName, FILE_READ );
	GameAssert( status == FILE_SUCCESS );

	// Read file size
	status = File::size( file, size );
	GameAssert( status == FILE_SUCCESS );

	char* data = new( TemporaryHeap::Instance(), ALIGN_4 ) char[size + 1];

	// Read from file
	status = File::read( file, data, size );
	GameAssert( status == FILE_SUCCESS );

	data[size] = 0;

	// Close file
	status = File::close( file );
	GameAssert( status == FILE_SUCCESS );

	return data;
}