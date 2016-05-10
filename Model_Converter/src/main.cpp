#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <Windows.h>

#include <cstring>
#include <cstdio>
#include <GameAssert.h>
#include <Logger.h>

#include "ErrorFormatter.h"
#include "ModelData.h"
#include "FBXImporter.h"
#include "ModelWriter.h"


void print_usage( const char* fullPath, bool displayFullHelp )
{
	const char* lastSlash = strrchr( fullPath, '\\' );
	const char* filename;

	if( lastSlash == 0 )
		filename = fullPath;
	else
		filename = lastSlash + 1;

	fprintf( stderr, "USAGE:\n" );
	fprintf( stderr, "   %s path\\to\\file.fbx [-include texture.tga]\n", filename );
	fprintf( stderr, "\n" );

	if( !displayFullHelp )
	{
		fprintf( stderr, "For additional command info, run with either -h or --help\n" );
	}
	else
	{
		fprintf( stderr, "\n" );
		fprintf( stderr, "PARAMETERS\n" );
		fprintf( stderr, "     file.fbx   The path to a file containing the FBX data to bake into an\n" );
		fprintf( stderr, "                archive. The file must have the .FBX file extension. Only one\n" );
		fprintf( stderr, "                FBX file may be specified for each execution of this command.\n" );
		fprintf( stderr, "                All texture data associated with the FBX file will be included\n" );
		fprintf( stderr, "                in the created archive. This parameter is required.\n" );
		fprintf( stderr, "\n" );
		fprintf( stderr, "\n" );
		fprintf( stderr, "OPTIONS\n" );
		fprintf( stderr, "     -include texture.tga   Optional. This switch may be used to include an\n" );
		fprintf( stderr, "                            additional texture in the model that is not in the\n" );
		fprintf( stderr, "                            fbx library. The texture file must be in the same\n" );
		fprintf( stderr, "                            directory as the FBX file.\n" );
		fprintf( stderr, "\n" );
		fprintf( stderr, "\n" );
		fprintf( stderr, "DESCRIPTION\n" );
		fprintf( stderr, "     This command will read the specified FBX asset file and bake it (as well as\n" );
		fprintf( stderr, "any other assets it uses) into a more runtime-friendly format. This command\n" );
		fprintf( stderr, "requires that the Vodka and Liu executables be in the same folder as this exe.\n" );
		fprintf( stderr, "\n" );
		fprintf( stderr, "\n" );
		fprintf( stderr, "REMARKS\n" );
		fprintf( stderr, "      The file name of the archive will be the filename of the provided FBX file\n" );
		fprintf( stderr, "with the '.fbx' extension changed to '.spu'. The package name in the archive\n" );
		fprintf( stderr, "meta info will be the value of the title field in the FBX's meta data, truncated\n" );
		fprintf( stderr, "to 19 characters if the title is longer. If the FBX file has no title in the\n" );
		fprintf( stderr, "meta data, then the package name will be the FBX's filename instead, minus the\n" );
		fprintf( stderr, "extension.\n" );
	}
}






void to_lower( char* str )
{
	while( *str != 0 )
	{
		if( *str >= 'A' && *str <= 'Z' )
			*str += 'a' - 'A';

		str++;
	}
}



void Parse_Path( const char* fbxFilePath, char*& path, char*& filename )
{
	char fullPath[MAX_PATH];
	GetFullPathName( fbxFilePath, MAX_PATH, fullPath, &filename );

	int filenameSize = strlen( filename );
	int pathSize = strlen( fullPath ) - filenameSize;
	GameAssert( strlen( fullPath ) > 0 );
	GameAssert( filenameSize > 0 );
	GameAssert( pathSize > 0 );
	GameAssert( fullPath[pathSize - 1] == '\\' );

	filename = new char[filenameSize + 1];
	strcpy_s( filename, filenameSize + 1, fullPath + pathSize );

	fullPath[pathSize] = 0;
	path = new char[pathSize + 1];
	strcpy_s( path, pathSize + 1, fullPath );
}



const static char* const MODEL_CONVERTER_VERSION = "1.1";


int main( int argc, char* argv[] )
{
	Logger::Start_Log( "converter.log", false );

	for( int i = 0; i < argc; i++ )
		to_lower( argv[i] );

	if( argc == 1 )
	{
		print_usage( argv[0], false );
		return ERROR_SUCCESS;
	}

	if( strcmp( argv[1], "--help" ) == 0 || strcmp( argv[1], "-h" ) == 0 )
	{
		print_usage( argv[0], true );
		return ERROR_SUCCESS;
	}

	if( argc > 4 )
	{
		Print_Error_Message( "Error: Too many arguments." );
		return ERROR_BAD_ARGUMENTS;
	}

	char* path;
	char* fbxFilename;

	Parse_Path( argv[1], path, fbxFilename );

	if( strlen( fbxFilename ) < 4 || strcmp( fbxFilename + strlen( fbxFilename ) - 4, ".fbx" ) != 0 )
	{
		Print_Error_Message( "Error: the FBX file argument does not have the proper file extension.\n" );
		return ERROR_BAD_ARGUMENTS;
	}

	if( strlen( fbxFilename ) == 4 )
	{
		Print_Error_Message( "Error: the FBX file argument must be more than just the extension.\n" );
		return ERROR_BAD_ARGUMENTS;
	}

	FBXImporter importer;
	importer.Import( path, fbxFilename );

	if( argc == 4 )
	{
		if( strcmp( argv[2], "-include" ) != 0 )
		{
			Print_Error_Message( "Error: unknown parameter/option.\n" );
			return ERROR_BAD_ARGUMENTS;
		}

		char* additionalTexture = argv[3];
		importer.Add_Extra_Texture( additionalTexture );
	}

	ModelWriter::Write( importer.GetModelData(), path, MODEL_CONVERTER_VERSION );

	Logger::Stop_Log();

	return ERROR_SUCCESS;
}