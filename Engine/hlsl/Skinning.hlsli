#ifndef _SKINNING_HEADER_
#define _SKINNING_HEADER_

#pragma pack_matrix( row_major )

#include "Input.hlsli"

inline float4 SkinVertex( float4 position,
						  uint boneIndex0, uint boneIndex1, uint boneIndex2, uint boneIndex3,
						  float boneWeight0, float boneWeight1, float boneWeight2, float boneWeight3 )
{
	float4 skinnedPosition = mul( mul( position, BindMatrices[boneIndex0] ), BoneMatrices[boneIndex0] ) * boneWeight0;
	skinnedPosition += mul( mul( position, BindMatrices[boneIndex1] ), BoneMatrices[boneIndex1] ) * boneWeight1;
	skinnedPosition += mul( mul( position, BindMatrices[boneIndex2] ), BoneMatrices[boneIndex2] ) * boneWeight2;
	skinnedPosition += mul( mul( position, BindMatrices[boneIndex3] ), BoneMatrices[boneIndex3] ) * boneWeight3;
	//float4 skinnedPosition = mul( mul( position, BoneMatrices[boneIndex0] ),BindMatrices[boneIndex0] ) * boneWeight0;
	//skinnedPosition += mul( mul( position, BoneMatrices[boneIndex1] ), BindMatrices[boneIndex1] ) * boneWeight1;
	//skinnedPosition += mul( mul( position, BoneMatrices[boneIndex2] ), BindMatrices[boneIndex2] ) * boneWeight2;
	//skinnedPosition += mul( mul( position, BoneMatrices[boneIndex3] ), BindMatrices[boneIndex3] ) * boneWeight3;

	//float4 skinnedPosition = mul( position, BoneMatrices[boneIndex0] ) * boneWeight0;
	//skinnedPosition += mul( position, BoneMatrices[boneIndex1] ) * boneWeight1;
	//skinnedPosition += mul( position, BoneMatrices[boneIndex2] ) * boneWeight2;
	//skinnedPosition += mul( position, BoneMatrices[boneIndex3] ) * boneWeight3;

	return skinnedPosition;
}

#endif // _SKINNING_HEADER_
