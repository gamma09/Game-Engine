#pragma pack_matrix( row_major )

#include "UnlitTexture.hlsli"
#include "PSInput.hlsli"

float4 main( PS_INPUT input ) : SV_TARGET
{
	return txDiffuse.Sample( samplerLinear, input.tc );
}