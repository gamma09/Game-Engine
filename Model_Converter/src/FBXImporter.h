#pragma once

#include <string>
#include <vector>

#include "Vertex.h"
#include "ModelData.h"

namespace fbxsdk
{
	class FbxManager;
	class FbxScene;
}

using namespace fbxsdk;
using namespace std;

class Vertex;

class FBXImporter
{
public:
	FBXImporter();
	~FBXImporter();
	FBXImporter( FBXImporter & ) = delete;
	FBXImporter & operator=( FBXImporter & ) = delete;

	void Add_Extra_Texture( const char* additionalTexture );
	void Import( const string& path, const string& filename );
	inline const ModelData& GetModelData() const { return this->modelData; }

private:
	void ImportMesh();
		void ImportTextures( FbxNode* node, FbxMesh* mesh );
		void ImportTriangle( FbxMesh* mesh, int polygonIndex );
			Vertex ImportVertex( FbxMesh* mesh, int polygonIndex, int vertexIndex );
				void ImportVertexUV( FbxMesh* mesh, int polygonIndex, int vertexIndex, Vertex& vtx ) const;
				void ImportVertexNormal( FbxMesh* mesh, int polygonIndex, int vertexIndex, Vertex& vtx ) const;
	void ImportSkeleton();
	void ImportAnimations();
	void ImportSkin();

private:
	ModelData modelData;

	FbxManager* manager;
	FbxScene* scene;
};

