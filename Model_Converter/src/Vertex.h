#pragma once

#include <vector>
#include <fbxsdk.h>

using namespace std;

#define BONE_INFLUENCE_COUNT 4

struct BoneInfluence
{
	float weight;
	unsigned int boneIndex;

	BoneInfluence() : weight( 0.0f ), boneIndex( 0 ) {}
	inline bool operator==( const BoneInfluence& other ) const { return weight == other.weight && boneIndex == other.boneIndex; }
};



class Vertex
{
public:
	Vertex();
	Vertex( const Vertex& v );
	Vertex& operator=( const Vertex& v );
	~Vertex();


	bool Equals( const Vertex& v, const float tolerance = 0.002f ) const;

	void Add_Bone_Influence( const BoneInfluence& influence );

	void Normalize_Bone_Influences();

public:

	// position
	float x;
	float y;
	float z;
	float w;

	// texture coordinates
	float u;
	float v;

	// surface normal
	float nx;
	float ny;
	float nz;
	float nw;

	vector<BoneInfluence> boneInfluences;

	int ControlPointIndex;
};



class SerializableVertex
{
public:
	SerializableVertex( const Vertex& vtx );
	SerializableVertex( const SerializableVertex& vtx );
	SerializableVertex& operator=( const SerializableVertex& vtx );
	~SerializableVertex();

	float x;
	float y;
	float z;
	float w;

	float u;
	float v;

	float nx;
	float ny;
	float nz;
	float nw;

	float boneWeight[BONE_INFLUENCE_COUNT];
	unsigned int boneIndex[BONE_INFLUENCE_COUNT];
};