#include "Input.hlsli"
#include "Skinning.hlsli"
#include "UnlitTexture.hlsli"

VS_OUTPUT main( VS_INPUT input )
{
	// Pre-computes the world * view * projection matrix (DirectX hlsl compiler will
	// actually move this calculation before the vertex shader, so that it will be computed
	// only once for every draw call)
	float4x4 preWorldViewProj = PreshaderComputeWorldViewProjection();

	float4 skinnedPosition = SkinVertex( input.position, input.boneIndices0, input.boneIndices1, input.boneIndices2, input.boneIndices3,
	                                     input.boneWeights0, input.boneWeights1, input.boneWeights2, input.boneWeights3 );

	VS_OUTPUT output;
	output.position = mul( skinnedPosition, preWorldViewProj );
	output.tc = input.tc;

	return output;
}
