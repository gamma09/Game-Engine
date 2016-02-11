#include <GameAssert.h>
#include <File.h>
#include "FileReader.h"
#include "SourceCodePipe.h"

FileReader::FileReader( SourceCodePipe* outputPipe )
:	out( outputPipe )
{
	GameAssert( outputPipe );
}

FileReader::~FileReader()
{
	this->out->Finish();
}

void FileReader::ReadFile( const char* filename )
{
	FileHandle fh;
	GameVerify( FILE_SUCCESS == File::open( fh, filename, FileMode::FILE_READ ) );
}