#include <d3d11_1.h>
#include <GameAssert.h>
#include <File.h>

#include "Material.h"
#include "GlobalHeaps.h"
#include "MemorySetup.h"
#include "ModelBaseManager.h"
#include "ModelBase.h"
#include "Model.h"
#include "Camera.h"
#include "DirectXAssert.h"
#include "FileLoader.h"







Material::Material()
{
	memset( this, 0, sizeof( Material ) );
}

void Material::Destroy()
{	
	if( this->inputLayout )
	{
		this->inputLayout->Release();
	}

	if( this->rasterizer )
	{
		this->rasterizer->Release();
	}
	
	if( this->vertexShader )
	{
		this->vertexShader->Release();
	}
	
	if( this->extraVertexShaderBuffers )
	{
		for( unsigned int i = 0; i < this->vertexShaderBufferCount; i++ )
		{
			this->extraVertexShaderBuffers[i]->Release();
		}

		delete this->extraVertexShaderBuffers;
	}

	if( this->pixelShader )
	{
		this->pixelShader->Release();
	}

	if( this->pixelShaderBuffers )
	{
		for( unsigned int i = 0; i < this->pixelShaderBufferCount; i++ )
		{
			this->pixelShaderBuffers[i]->Release();
		}

		delete this->pixelShaderBuffers;
	}
	
	if( this->pixelShaderSamplers )
	{
		for( unsigned int i = 0; i < this->pixelShaderSamplerCount; i++ )
		{
			this->pixelShaderSamplers[i]->Release();
		}

		delete this->pixelShaderSamplers;
	}
}

Material::~Material()
{
	this->Destroy();
}


Material::Material( const char* shaderFileBase, ID3D11Device* device )
	: inputLayout( nullptr ),
	vertexShader( nullptr ),
	extraVertexShaderBuffers( nullptr ),
	pixelShader( nullptr ),
	pixelShaderBuffers( nullptr ),
	pixelShaderSamplers( nullptr )
{
	GameAssert( shaderFileBase != nullptr );
	GameAssert( device != nullptr );
	char file[256];

	strcpy_s( file, shaderFileBase );
	strcat_s( file, ".vs.cso" );

	unsigned long size;
	char* vertexShaderData = FileLoader::Load_File( file, size );
	GameCheckFatalDx(  device->CreateVertexShader( vertexShaderData, size, nullptr, &this->vertexShader ), "Could not load compiled vertex shader." );

	SetupInputLayout( device, vertexShaderData, size );
	delete vertexShaderData;

	strcpy_s( file, shaderFileBase );
	strcat_s( file, ".ps.cso" );

	char* pixelShaderData = FileLoader::Load_File( file, size );
	GameCheckFatalDx(  device->CreatePixelShader( pixelShaderData, size, nullptr, &this->pixelShader ), "Could not load compiled pixel shader." );
	delete pixelShaderData;
}

#define INPUT_LAYOUT_DESC_COUNT 5
void Material::SetupInputLayout( ID3D11Device* device, const char* vertexShaderData, unsigned long dataSize )
{
	D3D11_INPUT_ELEMENT_DESC layout[INPUT_LAYOUT_DESC_COUNT] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDINDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, 56, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		
	};

	GameCheckFatalDx(  device->CreateInputLayout( layout, INPUT_LAYOUT_DESC_COUNT, vertexShaderData, dataSize, &this->inputLayout ), "Could not create vertex shader input layout." );
}

void Material::Draw( Camera* camera, const DirectionLight* light, ID3D11DeviceContext* context, ID3DUserDefinedAnnotation* annotation ) const
{
	annotation->BeginEvent( L"Shader Setup" );
	{
		context->IASetInputLayout( this->inputLayout );
		context->VSSetShader( this->vertexShader, nullptr, 0 );
		context->PSSetShader( this->pixelShader, nullptr, 0 );

		this->PrepareBuffers( context, light );
		context->VSSetConstantBuffers( 2, this->vertexShaderBufferCount, this->extraVertexShaderBuffers );
		context->PSSetConstantBuffers( 2, this->pixelShaderBufferCount, this->pixelShaderBuffers );
		context->PSSetSamplers( 0, this->pixelShaderSamplerCount, this->pixelShaderSamplers );

		context->RSSetState( this->rasterizer );
	}
	annotation->EndEvent();

	annotation->BeginEvent( L"Draw All ModelBases" );
	{
		for( auto modelBase = ModelBaseManager::Instance()->Active_Iterator(); !modelBase.Is_At_End(); modelBase++ )
		{
			static_cast<ModelBase&>( *modelBase ).Draw( context, this, camera, annotation );
		}
	}
	annotation->EndEvent();
}