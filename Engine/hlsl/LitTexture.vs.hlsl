#include "Input.hlsli"
#include "Skinning.hlsli"
#include "LitTexture.hlsli"


VS_OUTPUT main( VS_INPUT input )
{
	// Pre-computes the world * view * projection matrix (DirectX hlsl compiler will
	// actually move this calculation before the vertex shader, so that it will be computed
	// only once for every draw call)
	float4x4 preWorldViewProj = PreshaderComputeWorldViewProjection();

	float4 skinnedPosition = SkinVertex( input.position, input.boneIndices0, input.boneIndices1, input.boneIndices2, input.boneIndices3,
	                                     input.boneWeights0, input.boneWeights1, input.boneWeights2, input.boneWeights3 );

	// Calculates corrected vertex normals

	// model_view-Matrix will be calculated preshader
	float4x4 model_view_matrix = mul( World, View );
	float4x4 model_normal_matrix;
	model_normal_matrix._m00_m01_m02 = normalize( model_view_matrix._m00_m01_m02 );
	model_normal_matrix._m03 = 0.0;

	model_normal_matrix._m10_m11_m12 = normalize( model_view_matrix._m10_m11_m12 );
	model_normal_matrix._m13 = 0.0;

	model_normal_matrix._m20_m21_m22 = normalize( model_view_matrix._m20_m21_m22 );
	model_normal_matrix._m23 = 0.0;

	model_normal_matrix._m30_m31_m32_m33 = float4( 0.0, 0.0, 0.0, 1.0 );

	float4 normals;
	normals = normalize( mul( input.vertex_normal, model_normal_matrix ) );

	// get light direction and magnitude
	float4 ecPosition1;
	ecPosition1 = mul( input.position, model_view_matrix );
	float4 ecPosition2;
	ecPosition2 = ecPosition1 / ecPosition1.w;
	float4 light_direction;
	light_direction = normalize( LightPosition - ecPosition2 );

	// get fragment color
	float fdot = max( 0.0, dot( normals, light_direction ) );
	
	VS_OUTPUT output;
	output.diffuse_color.rgb = LightColor.rgb * fdot;
	output.diffuse_color.a = LightColor.a;
	output.position = mul( skinnedPosition, preWorldViewProj );
	output.texture_coordinate = input.tc;
	
	return output;
}