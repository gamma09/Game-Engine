#include "UnlitTexture.hlsli"
#include "Input.hlsli"

float4 main( PS_INPUT input ) : SV_TARGET
{
	return txDiffuse.Sample( samplerLinear, input.tc );
}