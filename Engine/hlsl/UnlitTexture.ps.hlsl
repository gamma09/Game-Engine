#pragma pack_matrix( row_major )

Texture2D txDiffuse : register( t0 );
SamplerState samplerLinear : register( s0 );

struct PS_INPUT
{
	float4 position : SV_POSITION;
	float2 tc : TEXCOORD0;
};

float4 main( PS_INPUT input ) : SV_TARGET
{
	return txDiffuse.Sample( samplerLinear, input.tc );
}