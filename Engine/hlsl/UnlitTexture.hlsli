#ifndef _UNLIT_TEXTURE_HEADER_
#define _UNLIT_TEXTURE_HEADER_

typedef struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float2 tc : TEXCOORD0;
} PS_INPUT;

#endif // _UNLIT_TEXTURE_HEADER
