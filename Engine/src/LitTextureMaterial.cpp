#include <d3d11_1.h>
#include <GameAssert.h>
#include <MathEngine.h>

#include "MemorySetup.h"
#include "LitTextureMaterial.h"
#include "ModelBase.h"
#include "Camera.h"
#include "DirectionLight.h"
#include "Texture.h"
#include "DirectXAssert.h"


LitTextureMaterial::LitTextureMaterial()
	: Material()
{
	// Do nothing
}

LitTextureMaterial::LitTextureMaterial( ID3D11Device* device )
	: Material( "LitTexture", device )
{
	SetupVertexShaderBuffers( device );
	SetupPixelShaderBuffers( device );
	SetupPixelShaderSamplers( device );
	SetupRasterizer( device );
}

LitTextureMaterial::~LitTextureMaterial()
{
	// Do nothing
}

#define LIGHT_DATA_BUFFER_INDEX 0

void LitTextureMaterial::PrepareBuffers( ID3D11DeviceContext* context, const DirectionLight* light ) const
{
	Vect lightData[2] ={ light->Get_Position(), light->Get_Color() };
	context->UpdateSubresource( this->extraVertexShaderBuffers[LIGHT_DATA_BUFFER_INDEX], 0, nullptr, lightData, 0, 0 );
}

#define EXTRA_VERTEX_SHADER_BUFFER_COUNT 1

void LitTextureMaterial::SetupVertexShaderBuffers( ID3D11Device* device )
{
	this->vertexShaderBufferCount = EXTRA_VERTEX_SHADER_BUFFER_COUNT;
	D3D11_BUFFER_DESC bd[EXTRA_VERTEX_SHADER_BUFFER_COUNT] =
	{
		{
			2 * sizeof( Vect ),
			D3D11_USAGE_DEFAULT,
			D3D11_BIND_CONSTANT_BUFFER,
			0,
			0,
			0
		}
	};

	this->extraVertexShaderBuffers = new( ConstantBufferHeap::Instance(), ALIGN_4 ) ID3D11Buffer*[EXTRA_VERTEX_SHADER_BUFFER_COUNT];
	for( unsigned int i = 0; i < EXTRA_VERTEX_SHADER_BUFFER_COUNT; i++ )
	{
		GameCheckFatalDx(  device->CreateBuffer( &bd[i], nullptr, &this->extraVertexShaderBuffers[i] ), "Could not create vertex shader's constant buffers." );
	}
}

#define PIXEL_SHADER_BUFFER_COUNT 0

void LitTextureMaterial::SetupPixelShaderBuffers( ID3D11Device* )
{
	this->pixelShaderBufferCount = PIXEL_SHADER_BUFFER_COUNT;
	this->pixelShaderBuffers = new( ConstantBufferHeap::Instance(), ALIGN_4 ) ID3D11Buffer*[PIXEL_SHADER_BUFFER_COUNT];
}

#define PIXEL_SHADER_SAMPLER_COUNT 1

void LitTextureMaterial::SetupPixelShaderSamplers( ID3D11Device* device )
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

void LitTextureMaterial::SetupRasterizer( ID3D11Device* device )
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
