#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <GameAssert.h>

#include "Vertex.h"
#include "DebugPrint.h"

SerializableVertex::SerializableVertex( const Vertex& vtx )
	: x( vtx.x ),
	y( vtx.y ),
	z( vtx.z ),
	w( vtx.w ),
	u( vtx.u ),
	v( vtx.v ),
	nx( vtx.nx ),
	ny( vtx.ny ),
	nz( vtx.nz ),
	nw( vtx.nw )
{
	GameAssert( vtx.boneInfluences.size() == BONE_INFLUENCE_COUNT );
	for( unsigned int i = 0; i < BONE_INFLUENCE_COUNT; i++ )
	{
		boneWeight[i] = vtx.boneInfluences[i].weight;
		boneIndex[i] = vtx.boneInfluences[i].boneIndex;
	}
}

SerializableVertex::SerializableVertex( const SerializableVertex& vtx )
{
	memcpy( this, &vtx, sizeof( SerializableVertex ) );
}

SerializableVertex& SerializableVertex::operator=( const SerializableVertex& vtx )
{
	memcpy( this, &vtx, sizeof( SerializableVertex ) );
	return *this;
}

SerializableVertex::~SerializableVertex()
{
}

Vertex::Vertex()
	: boneInfluences( BONE_INFLUENCE_COUNT )
{
}

Vertex::Vertex( const Vertex& v ) :
	x( v.x ),
	y( v.y ),
	z( v.z ),
	w( v.w ),
	u( v.u ),
	v( v.v ),
	nx( v.nx ),
	ny( v.ny ),
	nz( v.nz ),
	nw( v.nw ),
	boneInfluences( v.boneInfluences ),
	ControlPointIndex( v.ControlPointIndex )
{
}

Vertex& Vertex::operator=( const Vertex& v )
{
	this->x = v.x;
	this->y = v.y;
	this->z = v.z;
	this->w = v.w;
	this->u = v.u;
	this->v = v.v;
	this->nx = v.nx;
	this->ny = v.ny;
	this->nz = v.nz;
	this->nw = v.nw;
	this->boneInfluences = v.boneInfluences;
	this->ControlPointIndex = v.ControlPointIndex;

	return *this;
}

Vertex::~Vertex()
{
}

bool Vertex::Equals( const Vertex& v, const float tolerance ) const
{
	return fabsf( this->x - v.x ) < tolerance &&
		fabsf( this->y - v.y ) < tolerance &&
		fabsf( this->z - v.z ) < tolerance &&
		fabsf( this->w - v.w ) < tolerance &&
		fabsf( this->u - v.u ) < tolerance &&
		fabsf( this->v - v.v ) < tolerance &&
		fabsf( this->nx - v.nx ) < tolerance &&
		fabsf( this->ny - v.ny ) < tolerance &&
		fabsf( this->nz - v.nz ) < tolerance &&
		fabsf( this->nw - v.nw ) < tolerance &&
		this->boneInfluences == v.boneInfluences &&
		this->ControlPointIndex == v.ControlPointIndex;
}

static int Bone_Influence_Comparator( const void* a, const void* b )
{
	const BoneInfluence* boneA = static_cast<const BoneInfluence*>( a );
	const BoneInfluence* boneB = static_cast<const BoneInfluence*>( b );

	float diff = boneB->weight - boneA->weight;
	// this works because the first bit of the float indicates sign just like with integers, furthermore, 0x00000000 is 0 as an integer and is 0.0 as a float
	return *reinterpret_cast<int*>( &diff );
}

void Vertex::Add_Bone_Influence( const BoneInfluence& influence )
{
	boneInfluences.push_back( influence );
	
	qsort( boneInfluences.data(), boneInfluences.size(), sizeof( BoneInfluence ), Bone_Influence_Comparator );
	boneInfluences.resize( BONE_INFLUENCE_COUNT );
}

void Vertex::Normalize_Bone_Influences()
{
	float totalWeight = 0.0f;
	for( auto boneIterator = boneInfluences.cbegin(); boneIterator != boneInfluences.cend(); boneIterator++ )
	{
		totalWeight += boneIterator->weight;
	}

	float invWeight = ( totalWeight < 0.1f ) ? ( 1.0f ) : ( 1.0f / totalWeight );

	for( auto boneIterator = boneInfluences.begin(); boneIterator != boneInfluences.end(); boneIterator++ )
	{
		boneIterator->weight = boneIterator->weight * invWeight;
	}
}
