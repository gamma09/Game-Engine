#pragma once

#include <vector>
#include <memory>
#include "Vertex.h"
#include "Triangle.h"
#include "Texture.h"
#include "AnimationInfo.h"

using namespace std;


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

	void Set_Triangle_Texture( const unsigned int triangle, const short texture );

	void Set_Base_Filename( const char* fbxFilename );
	void Set_Title( const char* title );

	const char* Get_Base_Filename() const;
	const char* Get_Title() const;
	const vector<Vertex>& Get_Vertex_List() const;
	const vector<Triangle>& Get_Triangle_List() const;
	const vector<Texture>& Get_Texture_List() const;
	const vector<short>& Get_Triangle_Texture_List() const;
	AnimationInfo& Get_Animation_Info();

	void Get_Bounding_Sphere( float& radius, float& centerX, float& centerY, float& centerZ ) const;

private:

	char* filename;
	char title[20];

	AnimationInfo animInfo;

	vector<Vertex> vertexList;
	vector<Triangle> triangleList;
	vector<short> triangleTextureList;
	vector<Texture> textureList;

};