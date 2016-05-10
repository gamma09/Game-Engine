#include "ModelWriter.h"

#include <Shlwapi.h>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <GameAssert.h>

#include "Execute.h"

const static char* CHUNKIFIER_EXE = "Chunkify.exe";
const static char* UNIFIER_EXE = "Unify.exe";

// Model file format is in the file:
// Dave_Model_Format.txt

void ModelWriter::Write_Model( const ModelData& modelData, const char* path )
{
	char buf[MAX_PATH];
	sprintf_s( buf, "%s%s%s", path, modelData.Get_Base_Filename(), ".tmp" );

	FILE* modelBin;
	fopen_s( &modelBin, buf, "wb" );

	float radius;
	float centerX;
	float centerY;
	float centerZ;

	modelData.Get_Bounding_Sphere( radius, centerX, centerY, centerZ );

	fwrite( &radius, sizeof( float ), 1, modelBin );
	fwrite( &centerX, sizeof( float ), 1, modelBin );
	fwrite( &centerY, sizeof( float ), 1, modelBin );
	fwrite( &centerZ, sizeof( float ), 1, modelBin );

	const vector<Bone>& boneList = modelData.Get_Bones();
	unsigned int boneCount = boneList.size();

	const vector<Animation>& animList = modelData.Get_Animations();
	unsigned int animCount = animList.size();

	const vector<Vertex>& vtxList = modelData.Get_Vertex_List();
	unsigned int vtxCount = vtxList.size();

	const vector<Triangle>& triList = modelData.Get_Triangle_List();
	unsigned int triCount = triList.size();

	const vector<Texture>& textureList = modelData.Get_Texture_List();
	unsigned int textureCount = textureList.size();

	// Write the number of vertices
	fwrite( &vtxCount, sizeof( unsigned int ), 1, modelBin );

	// Write the number of triangles
	fwrite( &triCount, sizeof( unsigned int ), 1, modelBin );

	// Write the number of textures
	fwrite( &textureCount, sizeof( unsigned int ), 1, modelBin );

	// Write the number of bones
	fwrite( &boneCount, sizeof( unsigned int ), 1, modelBin );

	// Write the number of animations
	fwrite( &animCount, sizeof( unsigned int ), 1, modelBin );

	// Write all of the bone's parents
	for( auto it = boneList.cbegin(); it != boneList.cend(); it++ )
	{
		fwrite( &it->parentBone, sizeof( int ), 1, modelBin );
		fwrite( &it->invBindMatrix, sizeof( Matrix ), 1, modelBin );
	}

	// Write all of the animations
	for( auto it = animList.begin(); it != animList.end(); it++ )
	{
		Animation anim = *it;
		const vector<Frame>& frames = anim.Get_Animation_Frames();
		unsigned int frameCount = frames.size();

		// Write the # of frames
		fwrite( &frameCount, sizeof( unsigned int ), 1, modelBin );

		for( auto it2 = frames.cbegin(); it2 != frames.cend(); it2++ )
		{
			Frame frame = *it2;
			// Write the frame time in ms
			fwrite( &frame.frameTime, sizeof( unsigned int ), 1, modelBin );

			GameAssert( boneCount == frame.bones.size() );

			// Write the bone transform data
			fwrite( &frame.bones[0], sizeof( BoneFrame ), frame.bones.size(), modelBin );
		}
	}

	// Write all of the vertices
	for( auto it = vtxList.cbegin(); it != vtxList.cend(); it++ )
	{
		Vertex v = *it;
		SerializableVertex sv( v );
		fwrite( &sv, sizeof( SerializableVertex ), 1, modelBin );
	}

	// Write the triangles
	for( auto it = triList.cbegin(); it != triList.cend(); it++ )
	{
		Triangle tri = *it;
		fwrite( &tri, sizeof( Triangle ), 1, modelBin );
	}

	// Write the texture names as null-terminated strings
	for( auto it = textureList.cbegin(); it != textureList.cend(); it++ )
	{
		const Texture& texture = *it;
		fwrite( texture.Get_Texture_Name(), sizeof( char ), strlen( texture.Get_Texture_Name() ) + 1, modelBin );
	}

	fflush( modelBin );
	fclose( modelBin );

	HMODULE hModule = GetModuleHandle( NULL );
	char dir[MAX_PATH];
	GetModuleFileName( hModule, dir, MAX_PATH );
	PathRemoveFileSpec( dir );

	char executable[MAX_PATH];
	sprintf_s( executable, "%s\\%s", dir, CHUNKIFIER_EXE );

	sprintf_s( buf, "%s%s", modelData.Get_Base_Filename(), ".tmp" );
	char blo[MAX_PATH];
	sprintf_s( blo, "%s%s", modelData.Get_Base_Filename(), ".blo" );

	int size = 2 + strlen( buf ) + 1 + 2 + strlen( blo ) + 1 + strlen( "-t VERTS_TYPE -n" ) + 1 + 2 + strlen( modelData.Get_Title() ) + 1;
	char* command = new char[size];

	sprintf_s( command, size, "\"%s\" \"%s\" -t VERTS_TYPE -n \"%s\"", buf, blo, modelData.Get_Title() );

	GameVerify( 0 == Executor::Execute( executable, path, command ) );

	delete[] command;

	sprintf_s( buf, "%s%s%s", path, modelData.Get_Base_Filename(), ".tmp" );
	DeleteFile( buf );
}





void ModelWriter::Write_Texture( const Texture& texture, const char* path )
{
	char texturePath[MAX_PATH];
	sprintf_s( texturePath, "%s\\%s", path, texture.Get_Texture_File() );
	if( PathFileExists( texturePath ) )
	{
		char blo[MAX_PATH];
		sprintf_s( blo, "%s%s", texture.Get_Texture_File(), ".blo" );

		HMODULE hModule = GetModuleHandle( NULL );
		char dir[MAX_PATH];
		GetModuleFileName( hModule, dir, MAX_PATH );
		PathRemoveFileSpec( dir );

		char executable[MAX_PATH];
		sprintf_s( executable, "%s\\%s", dir, CHUNKIFIER_EXE );

		int size = 2 + strlen( texture.Get_Texture_File() ) + 1 + 2 + strlen( blo ) + 1 + strlen( "-t TEXTURE_TYPE -n" ) + 1 + 2 + strlen( texture.Get_Texture_Name() ) + 1;
		char* command = new char[size];


		sprintf_s( command, size, "\"%s\" \"%s\" -t TEXTURE_TYPE -n \"%s\"", texture.Get_Texture_File(), blo, texture.Get_Texture_Name() );

		GameVerify( 0 == Executor::Execute( executable, path, command ) );

		delete[] command;
	}
}





void ModelWriter::Write_Manifest( const ModelData& modelData, const char* path )
{
	char tmp[MAX_PATH];
	sprintf_s( tmp, "%s%s", path, "manifest.tmp" );

	FILE* tmpFile;
	fopen_s( &tmpFile, tmp, "wb" );

	fwrite( modelData.Get_Title(), strlen( modelData.Get_Title() ) + 1, 1, tmpFile );
	fflush( tmpFile );
	fclose( tmpFile );

	HMODULE hModule = GetModuleHandle( NULL );
	char dir[MAX_PATH];
	GetModuleFileName( hModule, dir, MAX_PATH );
	PathRemoveFileSpec( dir );

	char executable[MAX_PATH];
	sprintf_s( executable, "%s\\%s", dir, CHUNKIFIER_EXE );

	GameVerify( 0 == Executor::Execute( executable, path, "manifest.tmp manifest.blo -t MANIFEST_TYPE -n manifest" ) );

	DeleteFile( tmp );
}



void Clean_Up_Blo_Files( const char* path )
{
	char dir[MAX_PATH];
	GetCurrentDirectory( MAX_PATH, dir );

	SetCurrentDirectory( path );

	WIN32_FIND_DATA findData;
	HANDLE hFind = FindFirstFile( "*.blo", &findData );
	GameAssert( hFind != INVALID_HANDLE_VALUE );

	DeleteFile( findData.cFileName );

	while( FindNextFile( hFind, &findData ) )
		DeleteFile( findData.cFileName );

	GameAssert( GetLastError() == ERROR_NO_MORE_FILES );
	GameVerify( FindClose( hFind ) );
}

void ModelWriter::Write( const ModelData& modelData, const char* path, const char* version )
{
	Write_Manifest( modelData, path );

	Write_Model( modelData, path );

	const vector<Texture>& textureList = modelData.Get_Texture_List();
	for( const Texture& texture : textureList )
		Write_Texture( texture, path );

	HMODULE hModule = GetModuleHandle( NULL );
	char dir[MAX_PATH];
	GetModuleFileName( hModule, dir, MAX_PATH );
	PathRemoveFileSpec( dir );

	char executable[MAX_PATH];
	sprintf_s( executable, "%s\\%s", dir, UNIFIER_EXE );

	const char* archiveFilename = modelData.Get_Base_Filename();
	int size = 2 + strlen( archiveFilename ) + strlen( ".spu" ) + 1 + strlen( "-n" ) + 1 + 2 + strlen( modelData.Get_Title() ) + 1 + strlen( "-v" ) + 1 + 2 + strlen( version ) + 1;
	char* command = new char[size];
	sprintf_s( command, size, "\"%s.spu\" -n \"%s\" -v \"%s\"", archiveFilename, modelData.Get_Title(), version );

	GameVerify( 0 == Executor::Execute( executable, path, command ) );

	delete[] command;

	Clean_Up_Blo_Files( path );
}