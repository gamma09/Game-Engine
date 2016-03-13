#include <d3d11_1.h>
#include <GameAssert.h>
#include <MathEngine.h>

#include "MemorySetup.h"
#include "UnlitTextureMaterial.h"
#include "ModelBase.h"
#include "Camera.h"
#include "DrawInfo.h"
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
}

UnlitTextureMaterial::~UnlitTextureMaterial()
{
	// Do nothing
}

#define CAMERA_MATRICES_BUFFER_INDEX 0
#define TEXTURE_VIEW_RESOURCE_INDEX 0

void UnlitTextureMaterial::PrepareBuffers( const DrawInfo& info ) const
{
	// TODO glEnable( GL_CULL_FACE );
	// TODO glFrontFace( GL_CW );
	// TODO glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

	Matrix cameraMatrices[3] ={ info.camera->Get_View(), info.camera->Get_Projection(), *info.worldMatrix };
	info.context->UpdateSubresource( this->vertexShaderBuffers[CAMERA_MATRICES_BUFFER_INDEX], 0, nullptr, cameraMatrices, 0, 0 );
	ID3D11ShaderResourceView* textureView = info.texture->Get_Texture_Resource();
	info.context->PSSetShaderResources( TEXTURE_VIEW_RESOURCE_INDEX, 1, &textureView );
}

#define VERTEX_SHADER_BUFFER_COUNT 1

void UnlitTextureMaterial::SetupVertexShaderBuffers( ID3D11Device* device )
{
	this->vertexShaderBufferCount = VERTEX_SHADER_BUFFER_COUNT;
	D3D11_BUFFER_DESC bd[VERTEX_SHADER_BUFFER_COUNT] =
	{
		{
			3 * sizeof( Matrix ),
			D3D11_USAGE_DEFAULT,
			D3D11_BIND_CONSTANT_BUFFER,
			0,
			0,
			0
		}
	};

	this->vertexShaderBuffers = new( ConstantBufferHeap::Instance(), ALIGN_4 ) ID3D11Buffer*[VERTEX_SHADER_BUFFER_COUNT];
	for( unsigned int i = 0; i < VERTEX_SHADER_BUFFER_COUNT; i++ )
	{
		GameCheckFatal( SUCCEEDED( device->CreateBuffer( &bd[i], nullptr, &this->vertexShaderBuffers[i] ) ), "Could not create vertex shader's constant buffers." );
	}
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
		GameCheckFatal( SUCCEEDED( device->CreateSamplerState( &sd[i], &this->pixelShaderSamplers[i] ) ), "Could not create pixel shader's texture samplers." );
	}
}
