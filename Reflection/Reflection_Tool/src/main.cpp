#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <Windows.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <GameAssert.h>
#include <File.h>

void PrintUsage( const char* executable )
{
	fprintf( stderr, "Usage:\n" );
	fprintf( stderr, "  %s PROJECT_DIR OUT_DIR  \n", executable );
}

char* Read_File( const char* filename )
{
	FileHandle fh;
	GameVerify( FileError::FILE_SUCCESS == File::open( fh, filename, FileMode::FILE_READ ) );

	int length;
	GameVerify( FileError::FILE_SUCCESS == File::size( fh, length ) );
	char* str = new char[length + 1];
	str[length] = 0;
	GameVerify( FileError::FILE_SUCCESS == File::read( fh, str, length ) );
	GameVerify( FileError::FILE_SUCCESS == File::close( fh ) );

	return str;
}

std::string Format_Argument_String( const char* preamble, char* str, const char* delims )
{
	std::string outStr;

	char* current = strtok( str, ";" );
	if( current != nullptr )
	{
		outStr += preamble;
		outStr += current;

		current = strtok( nullptr, ";" );
	}

	while( current != nullptr )
	{
		outStr += ' ';
		outStr += preamble;
		outStr += current;

		current = strtok( nullptr, ";" );
	}

	return outStr;
}

int main( int argc, char** argv )
{
	if ( argc != 3 )
	{
		PrintUsage( argv[0] );
		return 1;
	}
	
	char* projectDir = argv[1];
	if( !SetCurrentDirectory( projectDir ) )
	{
		fprintf( stderr, "Could not set working directory to: %s\n", projectDir );
		return 1;
	}

#ifdef _DEBUG
	char* commandFile = "obj\\Debug\\Preprocess\\command.in";
	char* includeFile = "obj\\Debug\\Preprocess\\include.in";
	char* definesFile = "obj\\Debug\\Preprocess\\defines.in";
#else
	char* commandFile = "obj\\Release\\Preprocess\\command.in";
	char* includeFile = "obj\\Release\\Preprocess\\include.in";
	char* definesFile = "obj\\Release\\Preprocess\\defines.in";
#endif

	char* executable = Read_File( commandFile );
	char* includes = Read_File( includeFile );
	char* defines = Read_File( definesFile );

	std::string formattedIncludes = Format_Argument_String( "/I", includes, ";" );
	std::string formattedDefines = Format_Argument_String( "/D", includes, ";" );

	std::string commandLine = executable + formattedIncludes + " " + formattedDefines + " /P /EP ";


	char* outDir = argv[2];

}