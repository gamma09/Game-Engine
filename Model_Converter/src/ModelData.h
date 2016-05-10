#pragma once

#include <vector>
#include <memory>
#include "Vertex.h"
#include "Triangle.h"
#include "Texture.h"
#include "Animation.h"
#include "Bone.h"

using namespace std;

#define TITLE_SIZE 20

class ModelData
{
public:
	ModelData();
	ModelData( const ModelData& data );
	ModelData& operator=( const ModelData& data );
	~ModelData();


	// returns the index in the vertexList of the v (or a v look-alike)
	unsigned int Add_Vertex( const Vertex& v );
	void Add_Triangle( const Vertex& v0, const Vertex& v1, const Vertex& v2 );
	void Add_Texture( const char* textureName, const char* textureFile );
	int Add_Bone( const Bone& bone );
	void Add_Animation( const Animation& anim );
	void Add_Influence( int controlPointIndex, const BoneInfluence& influence );

	void Bind_Bone( unsigned int boneIndex, const Matrix& invBindMatrix );

	void Set_Triangle_Texture( const unsigned int triangle, const short texture );

	void Set_Base_Filename( const char* fbxFilename );
	void Set_Title( const char* title );

	const char* Get_Base_Filename() const;
	const char* Get_Title() const;
	const vector<Vertex>& Get_Vertex_List() const;
	const vector<Triangle>& Get_Triangle_List() const;
	const vector<Texture>& Get_Texture_List() const;
	const vector<short>& Get_Triangle_Texture_List() const;
	const vector<Bone>& Get_Bones() const;
	const vector<Animation> Get_Animations() const;

	void Get_Bounding_Sphere( float& radius, float& centerX, float& centerY, float& centerZ ) const;

	void Normalize_Influences();

private:

	char* filename;
	char title[TITLE_SIZE];

	vector<Vertex> vertexList;
	vector<Triangle> triangleList;
	vector<short> triangleTextureList;
	vector<Texture> textureList;
	vector<Animation> anims;
	vector<Bone> bones;

};