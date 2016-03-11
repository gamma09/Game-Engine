
//*****************************************************************************
// Shader "Uniforms" (they're called constant buffers in DirectX)
//*****************************************************************************

cbuffer cbViewMatrix : register( b0 )
{
	float4x4 View;
};

cbuffer cbProjectionMatrix : register( b1 )
{
	float4x4 Projection;
};

cbuffer cbWorldMatrix : register( b2 )
{
	float4x4 World;
};



//*****************************************************************************
// Vertex shader inputs and outputs
//*****************************************************************************

struct VS_INPUT
{
	float4 position : POSITION;
	float2 tc : TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float2 tc : TEXCOORD0;
};


VS_OUTPUT main( VS_INPUT input )
{
	VS_OUTPUT output;
	output.position = mul( mul( mul( input.position, World ), View ), Projection );
	output.tc = input.tc;

	return output;
}
