#include <d3d11_1.h>
#include <GameAssert.h>
#include <MathEngine.h>

#include "MemorySetup.h"
#include "WireframeMaterial.h"
#include "ModelBase.h"
#include "Camera.h"
#include "DirectionLight.h"
#include "DirectXAssert.h"

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
	SetupRasterizer( device );
}

WireframeMaterial::~WireframeMaterial()
{
	// Do nothing
}

#define CAMERA_MATRICES_BUFFER_INDEX 0

void WireframeMaterial::PrepareBuffers( ID3D11DeviceContext*, const DirectionLight* ) const
{
}

#define VERTEX_SHADER_BUFFER_COUNT 0

void WireframeMaterial::SetupVertexShaderBuffers( ID3D11Device* )
{
	this->vertexShaderBufferCount = VERTEX_SHADER_BUFFER_COUNT;
	this->extraVertexShaderBuffers = new( ConstantBufferHeap::Instance(), ALIGN_4 ) ID3D11Buffer*[VERTEX_SHADER_BUFFER_COUNT];
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

void WireframeMaterial::SetupRasterizer( ID3D11Device* device )
{
	D3D11_RASTERIZER_DESC rasterizerDesc;
	rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	rasterizerDesc.FrontCounterClockwise = TRUE;
	rasterizerDesc.DepthBias = 1;
	rasterizerDesc.DepthBiasClamp = 1.0f;
	rasterizerDesc.SlopeScaledDepthBias = 0.0f;
	rasterizerDesc.DepthClipEnable = FALSE;
	rasterizerDesc.ScissorEnable = FALSE;
	rasterizerDesc.MultisampleEnable = FALSE;
	rasterizerDesc.AntialiasedLineEnable = TRUE;

	GameCheckFatalDx( device->CreateRasterizerState( &rasterizerDesc, &this->rasterizer ), "Could not create the rasterizer state." );
}
