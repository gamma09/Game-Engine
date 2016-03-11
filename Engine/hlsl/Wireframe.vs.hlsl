
//*****************************************************************************
// Shader "Uniforms" (they're called constant buffers in DirectX)
//*****************************************************************************

cbuffer : register( b0 )
{
	float4x4 View;
};

cbuffer : register( b1 )
{
	float4x4 Projection;
};

cbuffer : register( b2 )
{
	float4x4 World;
};

float4 main( float4 pos : POSITION ) : SV_POSITION
{
	return mul( mul( mul( pos, World ), View ), Projection );
}
