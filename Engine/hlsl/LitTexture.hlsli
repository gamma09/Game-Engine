#ifndef _LIT_TEXTURE_HEADER_
#define _LIT_TEXTURE_HEADER_

#pragma pack_matrix( row_major )

//*****************************************************************************
// Shader "Uniforms" (they're called constant buffers in DirectX)
//*****************************************************************************
cbuffer cbLightData : register( b5 )
{
	float4 LightPosition;
	float4 LightColor;
};

typedef struct VS_OUTPUT
{
	float4 position : VS_POSITION;
	float4 diffuse_color : COLOR;
	float2 texture_coordinate : TEXCOORD;
} PS_INPUT;

#endif // _LIT_TEXTURE_HEADER_
