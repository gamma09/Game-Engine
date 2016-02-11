#pragma once

class SourceCodePipe;

class FileReader
{
public:
	FileReader( SourceCodePipe* outputPipe );
	~FileReader();
	
	// Reads the file with the filename specified and writes the contents
	// to the source code pipe
	void ReadFile( const char* filename );
	
private:
	SourceCodePipe* out;
	
	
private:
	FileReader( const FileReader& reader ) = delete;
	FileReader( FileReader&& reader ) = delete;
	FileReader& operator=( const FileReader& reader ) = delete;
	FileReader& operator=( FileReader&& reader ) = delete;
};