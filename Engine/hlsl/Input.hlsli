#ifndef _INPUT_HEADER_
#define _INPUT_HEADER_

#pragma pack_matrix( row_major )

//*****************************************************************************
// Constants
//*****************************************************************************

// 1024 is the total number of matrices that can fit in a single register
// However, we will only support up to 256 (for now)
#define MAX_BONE_COUNT 256



//*****************************************************************************
// Shader "Uniforms" (they're called constant buffers in DirectX)
//*****************************************************************************

cbuffer cbNeverChanges : register( b0 )
{
	float4x4 Projection;
};

cbuffer cbChangesOncePerFrame : register( b1 )
{
	float4x4 View;
};

cbuffer cbRebindsEveryModelBase : register( b2 )
{
	float4x4 BindMatrices[MAX_BONE_COUNT];
};

// Changes extremely often (like many times every frame)
cbuffer cbWorldMatrix : register( b3 )
{
	float4x4 World;
};

cbuffer cbBoneMatrices : register( b4 )
{
	float4x4 BoneMatrices[MAX_BONE_COUNT];
}


Texture2D txDiffuse : register( t0 );

SamplerState samplerLinear : register( s0 );



//*****************************************************************************
// Vertex Shader Input (this is the same for ALL shaders)
//*****************************************************************************
struct VS_INPUT
{
	float4 position : POSITION;
	float2 tc : TEXCOORD;
	float4 vertex_normal : NORMAL;
	float boneWeights0 : BLENDWEIGHT0;
	float boneWeights1 : BLENDWEIGHT1;
	float boneWeights2 : BLENDWEIGHT2;
	float boneWeights3 : BLENDWEIGHT3;
	uint boneIndices0 : BLENDINDICES0;
	uint boneIndices1 : BLENDINDICES1;
	uint boneIndices2 : BLENDINDICES2;
	uint boneIndices3 : BLENDINDICES3;
};


//*****************************************************************************
// Helper functions for pre-computing inputs together
//*****************************************************************************
inline float4x4 PreshaderComputeWorldViewProjection()
{
	return mul( World, mul( View, Projection ) );
}

#endif // _INPUT_HEADER_
