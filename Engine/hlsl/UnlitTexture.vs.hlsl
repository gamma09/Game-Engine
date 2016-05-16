#pragma pack_matrix( row_major )

#include "VSInput.hlsli"
#include "UnlitTexture.hlsli"

VS_OUTPUT main( VS_INPUT input )
{
	VS_OUTPUT output;
	output.position = SkinVertex( input.position, input.boneIndices, input.boneWeights );
	output.tc = input.tc;

	return output;
}
