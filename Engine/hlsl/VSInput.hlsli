#ifndef _VS_INPUT_HEADER_
#define _VS_INPUT_HEADER_

#include "SharedShaderDefines.h"

//*****************************************************************************
// Shader resources
//*****************************************************************************
cbuffer cbNeverChanges : register( b0 )
{
	float4x4 invProjection;
};


StructuredBuffer<float4x4> BoneMatrices : register( t0 );



//*****************************************************************************
// Vertex Shader Input (this is the same for ALL shaders)
//*****************************************************************************
struct VS_INPUT
{
	float4 position : POSITION;
	float2 tc : TEXCOORD;
	float4 vertex_normal : NORMAL;
	float4 boneWeights : BLENDWEIGHT;
	uint4 boneIndices : BLENDINDICES;
};

inline float4 SkinVertex( float4 position, uint4 boneIndex, float4 boneWeight )
{
	float4 skinnedPosition = mul( position, BoneMatrices[boneIndex[0]] ) * boneWeight[0];
	skinnedPosition += mul( position, BoneMatrices[boneIndex[1]] ) * boneWeight[1];
	skinnedPosition += mul( position, BoneMatrices[boneIndex[2]] ) * boneWeight[2];
	skinnedPosition += mul( position, BoneMatrices[boneIndex[3]] ) * boneWeight[3];

	return skinnedPosition;
}

#endif // _VS_INPUT_HEADER_
