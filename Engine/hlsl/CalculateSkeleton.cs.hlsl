#pragma pack_matrix( row_major )

#include "SharedShaderDefines.h"
#include "Interpolation.hlsli"

#define THREAD_COUNT_X 16
#define THREAD_COUNT_Y 8
#define THREAD_COUNT_Z 1

// Check to make sure the thread group is large enough to handle all of the bones
#if THREAD_COUNT_X * THREAD_COUNT_Y * THREAD_COUNT_Z != MAX_BONES
#error The number of threads in one thread group must be equal to the maximum number of bones as defined by SharedShaderDefines.h(5).
#endif

cbuffer cbChangesRarely : register( CS_VIEW_PROJECTION_REGISTER )
{
	float4x4 ViewProjection;
};

cbuffer cbHierarchyChangesEveryModelBase : register( CS_HIERARCHY_REGISTER )
{
	// Remember, this is padded between each value, so it would be the same as:
	// int BoneParent0;
	// int3 Padding; 
	// int BoneParent1;
	// int3 Padding;
	// ...
	int BoneParents[MAX_BONES];
};

cbuffer cbBindMatricesChangesEveryModelBase : register( CS_BIND_POSE_REGISTER )
{
	float4x4 BindMatrices[MAX_BONES];
};

cbuffer cbChangesEveryModel : register( CS_MODEL_DATA_REGISTER )
{
	float4x4 World;
	float InterpolationTime;
};

struct Bone
{
	float4 Rotation;
	float4 Translation;
	float4 Scale;
};

StructuredBuffer<Bone> FrameA : register( CS_PREV_FRAME_REGISTER );
StructuredBuffer<Bone> FrameB : register( CS_NEXT_FRAME_REGISTER );
RWStructuredBuffer<float4x4> FinalBoneMatrices : register( CS_OUTPUT_BONES_REGISTER );


// 64 * 128 = 8192 (8 KB)
groupshared float4x4 IntermediateBoneMatrices[MAX_BONES];


[numthreads(THREAD_COUNT_X, THREAD_COUNT_Y, THREAD_COUNT_Z)]
void main( uint index : SV_GroupIndex )
{
	//------------------------------------------------------------------------
	// Part 1: Interpolate bones between frames and generate bone matrices
	// (which are in each bone's parent space)
	//------------------------------------------------------------------------

	float4x4 rotationMtx = InterpolateRotation( FrameA[index].Rotation, FrameB[index].Rotation, InterpolationTime );
	float4x4 translationMtx = InterpolateTranslation( FrameA[index].Translation, FrameB[index].Translation, InterpolationTime );
	float4x4 scaleMtx = InterpolationScale( FrameA[index].Scale, FrameB[index].Scale, InterpolationTime );

	IntermediateBoneMatrices[index] = mul( scaleMtx, mul( rotationMtx, translationMtx ) );

	// Wait for all compute threads to finish with Part 1
	GroupMemoryBarrierWithGroupSync();



	//------------------------------------------------------------------------
	// Part 2: Calculate bone matrices in device space (this includes walking
	// the bone hierarchy, followed by multiplying by the World-View-Projection
	// matrix)
	//------------------------------------------------------------------------

	int parentIndex = BoneParents[index];
	float4x4 result = IntermediateBoneMatrices[index];

	// Walk the bone hierarchy
	while( parentIndex != -1 )
	{
		result = mul( result, IntermediateBoneMatrices[parentIndex] );
		parentIndex = BoneParents[parentIndex];
	}

	FinalBoneMatrices[index] = mul( mul( BindMatrices[index], result ), mul( World, ViewProjection ) );
}