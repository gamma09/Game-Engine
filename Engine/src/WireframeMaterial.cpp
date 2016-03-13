#include <d3d11_1.h>
#include <GameAssert.h>
#include <MathEngine.h>

#include "MemorySetup.h"
#include "WireframeMaterial.h"
#include "ModelBase.h"
#include "Camera.h"
#include "DirectionLight.h"
#include "DrawInfo.h"

WireframeMaterial::WireframeMaterial()
	: Material()
{
	// Do nothing
}

WireframeMaterial::WireframeMaterial( ID3D11Device* device )
	: Material( "Wireframe", device )
{
	SetupVertexShaderBuffers( device );
	SetupPixelShaderBuffers( device );
	SetupPixelShaderSamplers( device );
}

WireframeMaterial::~WireframeMaterial()
{
	// Do nothing
}

#define CAMERA_MATRICES_BUFFER_INDEX 0

void WireframeMaterial::PrepareBuffers( const DrawInfo& info ) const
{
	// TODO glDisable(GL_CULL_FACE);
	// TODO glFrontFace(GL_CW);
	// TODO glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	Matrix cameraMatrices[3] ={ info.camera->Get_View(), info.camera->Get_Projection(), *info.worldMatrix };
	info.context->UpdateSubresource( this->vertexShaderBuffers[CAMERA_MATRICES_BUFFER_INDEX], 0, nullptr, cameraMatrices, 0, 0 );
}

#define VERTEX_SHADER_BUFFER_COUNT 1

void WireframeMaterial::SetupVertexShaderBuffers( ID3D11Device* device )
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

void WireframeMaterial::SetupPixelShaderBuffers( ID3D11Device* )
{
	this->pixelShaderBufferCount = PIXEL_SHADER_BUFFER_COUNT;
	this->pixelShaderBuffers = new( ConstantBufferHeap::Instance(), ALIGN_4 ) ID3D11Buffer*[PIXEL_SHADER_BUFFER_COUNT];
}

#define PIXEL_SHADER_SAMPLER_COUNT 0

void WireframeMaterial::SetupPixelShaderSamplers( ID3D11Device* )
{
	this->pixelShaderSamplerCount = PIXEL_SHADER_SAMPLER_COUNT;
	this->pixelShaderSamplers = new( ConstantBufferHeap::Instance(), ALIGN_4 ) ID3D11SamplerState*[PIXEL_SHADER_SAMPLER_COUNT];
}
