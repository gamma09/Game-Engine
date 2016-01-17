#pragma once

class ModelData;

#pragma warning( push, 3 )
#include <fbxsdk.h>
#pragma warning( pop )



class Vertex;

class FBXReader
{
public:
	// Path must end with a directory separator
	static void Read( ModelData& modelData, const char* path, const char* fbxFilename );

private:

	static void Read_Animations( ModelData& modelData, FbxScene* scene );
	static void Read_Bone( ModelData& modelData, const char* parentName, FbxNode* boneNode, int level );
	static void Read_Mesh( ModelData& modelData, const char* name, FbxNode* meshNode );

	static void Read_Hierarchy( ModelData& modelData, FbxScene* scene );
	static void Read_Textures( ModelData& modelData, FbxMesh* mesh );

	static void Read_Triangles( ModelData& modelData, FbxMesh* mesh );
	static Vertex Read_Vertex( FbxMesh* mesh, FbxVector4* controlPoints, const int polygon, const int vertex );
};