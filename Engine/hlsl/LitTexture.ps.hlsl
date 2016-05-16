#pragma pack_matrix( row_major )

#include "LitTexture.hlsli"
#include "PSInput.hlsli"

float4 main( PS_INPUT input ) : SV_TARGET
{
	return input.diffuse_color * txDiffuse.Sample( samplerLinear, input.texture_coordinate );
}