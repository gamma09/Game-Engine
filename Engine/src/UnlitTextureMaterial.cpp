#include <d3d11_1.h>
#include <GameAssert.h>
#include <MathEngine.h>

#include "DirectXAssert.h"
#include "MemorySetup.h"
#include "UnlitTextureMaterial.h"
#include "ModelBase.h"
#include "Model.h"
#include "Camera.h"
#include "Texture.h"

UnlitTextureMaterial::UnlitTextureMaterial()
	: Material()
{
	// Do nothing
}

UnlitTextureMaterial::UnlitTextureMaterial( ID3D11Device* device )
	: Material( "UnlitTexture", device )
{
	SetupVertexShaderBuffers( device );
	SetupPixelShaderBuffers( device );
	SetupPixelShaderSamplers( device );
	SetupRasterizer( device );
}

UnlitTextureMaterial::~UnlitTextureMaterial()
{
	// Do nothing
}


#define TEXTURE_VIEW_RESOURCE_INDEX 0

void UnlitTextureMaterial::PrepareBuffers( ID3D11DeviceContext*, const DirectionLight* ) const
{
}

#define VERTEX_SHADER_BUFFER_COUNT 0

void UnlitTextureMaterial::SetupVertexShaderBuffers( ID3D11Device* )
{
	this->vertexShaderBufferCount = VERTEX_SHADER_BUFFER_COUNT;
	this->extraVertexShaderBuffers = new( ConstantBufferHeap::Instance(), ALIGN_4 ) ID3D11Buffer*[VERTEX_SHADER_BUFFER_COUNT];
}

#define PIXEL_SHADER_BUFFER_COUNT 0

void UnlitTextureMaterial::SetupPixelShaderBuffers( ID3D11Device* )
{
	this->pixelShaderBufferCount = PIXEL_SHADER_BUFFER_COUNT;
	this->pixelShaderBuffers = new( ConstantBufferHeap::Instance(), ALIGN_4 ) ID3D11Buffer*[PIXEL_SHADER_BUFFER_COUNT];
}

#define PIXEL_SHADER_SAMPLER_COUNT 1

void UnlitTextureMaterial::SetupPixelShaderSamplers( ID3D11Device* device )
{
	this->pixelShaderSamplerCount = PIXEL_SHADER_SAMPLER_COUNT;
	D3D11_SAMPLER_DESC sd[PIXEL_SHADER_SAMPLER_COUNT] =
	{
		{
			D3D11_FILTER_MIN_MAG_MIP_LINEAR,
			D3D11_TEXTURE_ADDRESS_WRAP,
			D3D11_TEXTURE_ADDRESS_WRAP,
			D3D11_TEXTURE_ADDRESS_WRAP,
			0,
			0,
			D3D11_COMPARISON_NEVER,
			{ 0.0f, 0.0f, 0.0f, 1.0f },
			0.0f,
			D3D11_FLOAT32_MAX
		}
	};

	this->pixelShaderSamplers = new( ConstantBufferHeap::Instance(), ALIGN_4 ) ID3D11SamplerState*[PIXEL_SHADER_SAMPLER_COUNT];

	for( unsigned int i = 0; i < PIXEL_SHADER_SAMPLER_COUNT; i++ )
	{
		GameCheckFatalDx(  device->CreateSamplerState( &sd[i], &this->pixelShaderSamplers[i] ), "Could not create pixel shader's texture samplers." );
	}
}

void UnlitTextureMaterial::SetupRasterizer( ID3D11Device* device )
{
	D3D11_RASTERIZER_DESC rasterizerDesc;
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	rasterizerDesc.FrontCounterClockwise = TRUE;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0.0f;
	rasterizerDesc.SlopeScaledDepthBias = 0.0f;
	rasterizerDesc.DepthClipEnable = FALSE;
	rasterizerDesc.ScissorEnable = FALSE;
	rasterizerDesc.MultisampleEnable = TRUE;
	rasterizerDesc.AntialiasedLineEnable = FALSE;

	GameCheckFatalDx( device->CreateRasterizerState( &rasterizerDesc, &this->rasterizer ), "Could not create the rasterizer state." );
}
