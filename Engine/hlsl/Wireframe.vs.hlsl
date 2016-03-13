
//*****************************************************************************
// Shader "Uniforms" (they're called constant buffers in DirectX)
//*****************************************************************************

cbuffer cbCameraMatrices : register( b0 )
{
	float4x4 View;
	float4x4 Projection;
	float4x4 World;
};

struct VS_INPUT
{
	float4 position : POSITION;
	float2 tc : TEXCOORD0;         // unused
	float4 vertex_normal : NORMAL; // unused
};

float4 main( VS_INPUT input ) : SV_POSITION
{
	return mul( mul( mul( input.position, World ), View ), Projection );
}
