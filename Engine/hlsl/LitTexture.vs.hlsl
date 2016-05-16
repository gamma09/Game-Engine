#pragma pack_matrix( row_major )

#include "VSInput.hlsli"
#include "LitTexture.hlsli"

cbuffer cbLightData : register( VS_LIT_TEXTURE_LIGHT_REGISTER )
{
	float4 LightPosition;
	float4 LightColor;
};

VS_OUTPUT main( VS_INPUT input )
{
	float4 position = SkinVertex( input.position, input.boneIndices, input.boneWeights );

	// Calculates corrected vertex normal
	float4 normal = mul( SkinVertex( input.vertex_normal, input.boneIndices, input.boneWeights ), invProjection );

	// get light direction and magnitude
	float4 ecPosition1 = mul( position, invProjection );
	float4 ecPosition2 = ecPosition1 / ecPosition1.w;
	float4 light_direction = normalize( LightPosition - ecPosition2 );

	// get fragment color
	float fdot = max( 0.0f, dot( normal, light_direction ) );
	
	VS_OUTPUT output;
	output.diffuse_color.rgb = LightColor.rgb * fdot;
	output.diffuse_color.a = LightColor.a;
	output.position = position;
	output.texture_coordinate = input.tc;
	
	return output;
}