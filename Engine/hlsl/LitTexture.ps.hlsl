#pragma pack_matrix( row_major )

Texture2D txDiffuse : register( t0 );
SamplerState samplerLinear : register( s0 );

struct PS_INPUT
{
	float4 position : VS_POSITION;
	float4 diffuse_color : COLOR;
	float2 texture_coordinate : TEXCOORD;
};

float4 main( PS_INPUT input ) : SV_TARGET
{
	return input.diffuse_color * txDiffuse.Sample( samplerLinear, input.texture_coordinate );
}