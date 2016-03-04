#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <Windows.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <GameAssert.h>
#include <File.h>

#include "FilePreprocessor.h"
#include "ClassFilter.h"
#include "CodeParser.h"
#include "ReflectionData.h"
#include "DataWriter.h"
#include "FeedbackContext.h"

void PrintUsage( const char* executable )
{
	fprintf( stderr, "Usage:\n" );
	fprintf( stderr, "  %s SOLUTION_DIR PROJECT_DIR OUT_DIR CL_PATH DEFINES INCLUDES PROJECT_CONFIGURATION \n", executable );
}





std::string Format_Argument_String( const char* preamble, char* str, const char* postamble, const char* delims )
{
	std::string outStr;

	char* current = strtok( str, delims );
	if( current != nullptr )
	{
		outStr += preamble;
		outStr += current;
		outStr += postamble;

		current = strtok( nullptr, delims );
	}

	while( current != nullptr )
	{
		outStr += ' ';
		outStr += preamble;
		outStr += current;
		outStr += postamble;

		current = strtok( nullptr, delims );
	}

	return outStr;
}





int main( int argc, char** argv )
{
#ifdef _DEBUG
	while( !IsDebuggerPresent() )
		Sleep( 10 );
#endif

	cerr << "Command line: " << argc << ": ";
	for( int i = 0; i < argc; i++ )
	{
		cerr << argv[i] << " ";
	}

	cerr << endl;

	if ( argc != 8 )
	{
		PrintUsage( argv[0] );
		return 1;
	}
	
	char* solutionDir = argv[1];
	char* projectDir = argv[2];
	if( !SetCurrentDirectory( projectDir ) )
	{
		fprintf( stderr, "Could not set working directory to: %s\n", projectDir );
		return 1;
	}

	std::string preprocFile = std::string("obj\\") + argv[7] + "\\preprocess.cpp";
	std::string srcmlFile   = std::string("obj\\") + argv[7] + "\\srcml.xml";

	char* executable = argv[4];
	char* defines = argv[5];
	char* includes = argv[6];

	std::string formattedIncludes = Format_Argument_String( "/I \"", includes, "\"", ";" );
	std::string formattedDefines = Format_Argument_String( "/D \"", defines, "\"", ";" );

	std::string cl = std::string( "\"" ) + executable + "\" " + formattedIncludes + " " + formattedDefines + " /nologo /P /EP /Fi" + preprocFile + " ";
	std::string srcml = std::string( solutionDir ) + "\\src2srcml\\srcml.exe -o " + srcmlFile + " " + preprocFile;

	cerr << "cl = " << cl << endl;
	cerr << "srcml = " << srcml.c_str() << endl;

	GameAssert( srcml[0] != '(' );

	ReflectionData data;
	FilePreprocessor* preprocessor = new FilePreprocessor( new ClassFilter( new CodeParser( &data ) ) );

	FeedbackContext context( std::cerr );

	WIN32_FIND_DATA foundFileData;
	HANDLE hFileSearch = FindFirstFile( "src\\*.h", &foundFileData );
	if( hFileSearch != INVALID_HANDLE_VALUE )
	{
		BOOL bSuccess = TRUE;
		while( bSuccess )
		{
			preprocessor->PreprocessFile( cl, srcml, std::string( "src\\" ) + foundFileData.cFileName, srcmlFile.c_str(), context );

			bSuccess = FindNextFile( hFileSearch, &foundFileData );
		}

		FindClose( hFileSearch );
	}

	data.CheckVariableTypeLinks( context );

	std::string outputFile = std::string( argv[3] ) + "\\GeneratedReflectionCode.cpp";
	Write_Reflection_Data( outputFile.c_str(), &data );

	return 0;
}