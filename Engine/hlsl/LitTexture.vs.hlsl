

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

cbuffer cbLightData : register( b3 )
{
	float4 LightPosition;
	float4 LightColor;
};


//*****************************************************************************
// Vertex shader inputs and outputs
//*****************************************************************************
struct VS_INPUT
{
	float4 vertex : POSITION;
	float2 texture_coordinate : TEXCOORD;
	float4 vertex_normal : NORMAL;
};

struct VS_OUTPUT
{
	float4 position : VS_POSITION;
	float4 diffuse_color : COLOR;
	float2 texture_coordinate : TEXCOORD;
};

VS_OUTPUT main( VS_INPUT input )
{
	// Calculates corrected vertex normals
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
	ecPosition1 = mul( input.vertex, model_view_matrix );
	float4 ecPosition2;
	ecPosition2 = ecPosition1 / ecPosition1.w;
	float4 light_direction;
	light_direction = normalize( LightPosition - ecPosition2 );

	// get fragment color
	float fdot = max( 0.0, dot( normals, light_direction ) );
	
	VS_OUTPUT output;
	output.diffuse_color.rgb = LightColor.rgb * fdot;
	output.diffuse_color.a = LightColor.a;
	output.position = mul( mul( input.vertex, model_view_matrix ), Projection );
	output.texture_coordinate = input.texture_coordinate;
	
	return output;
}