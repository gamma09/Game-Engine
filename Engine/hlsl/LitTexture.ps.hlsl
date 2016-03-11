
Texture2D txDiffuse : register( t0 );
SamplerState samplerLinear : register( s0 );

struct PS_INPUT
{
	float4 position : POSITION;
	float4 diffuse_color : COLOR;
	float2 texture_coordinate : TEXCOORD;
};

float4 main( PS_INPUT input ) : SV_TARGET
{
	return input.diffuse_color * txDiffuse( samplerLinear, input.texture_coordinate );
}