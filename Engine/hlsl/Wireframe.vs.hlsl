#pragma pack_matrix( row_major )

#include "VSInput.hlsli"

float4 main( VS_INPUT input ) : SV_POSITION
{
	return SkinVertex( input.position, input.boneIndices, input.boneWeights );
}
