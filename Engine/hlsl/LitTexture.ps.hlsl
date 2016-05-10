#include "LitTexture.hlsli"
#include "Input.hlsli"

float4 main( PS_INPUT input ) : SV_TARGET
{
	return input.diffuse_color * txDiffuse.Sample( samplerLinear, input.texture_coordinate );
}