
//*****************************************************************************
// Shader "Uniforms" (they're called constant buffers in DirectX)
//*****************************************************************************
#pragma pack_matrix( row_major )

cbuffer cbCameraMatrices : register( b0 )
{
	float4x4 View;
	float4x4 Projection;
	float4x4 World;
};



//*****************************************************************************
// Vertex shader inputs and outputs
//*****************************************************************************

struct VS_INPUT
{
	float4 position : POSITION;
	float2 tc : TEXCOORD0;
	float4 vertex_normal : NORMAL; // unused
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
