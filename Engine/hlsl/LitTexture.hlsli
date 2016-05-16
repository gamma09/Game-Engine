#ifndef _LIT_TEXTURE_HEADER_
#define _LIT_TEXTURE_HEADER_

typedef struct VS_OUTPUT
{
	float4 position : VS_POSITION;
	float4 diffuse_color : COLOR;
	float2 texture_coordinate : TEXCOORD;
} PS_INPUT;

#endif // _LIT_TEXTURE_HEADER_
