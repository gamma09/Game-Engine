#include <d3d11_1.h>
#include <GameAssert.h>
#include <File.h>

#include "Material.h"
#include "GlobalHeaps.h"
#include "MemorySetup.h"
#include "DrawInfo.h"
#include "ModelBase.h"


//---------------------------------------------------------------------------------------------------------
// Internal methods
//---------------------------------------------------------------------------------------------------------

static char* Load_File( const char* fileName, unsigned long& size )
{
	GameAssert( fileName != 0 );

	FileHandle file;
	FileError status;

	status = File::open( file, fileName, FILE_READ );
	GameAssert( status == FILE_SUCCESS );

	// Read file size
	status = File::size( file, size );
	GameAssert( status == FILE_SUCCESS );

	char* data = new( TemporaryHeap::Instance(), ALIGN_4 ) char[size + 1];

	// Read from file
	status = File::read( file, data, size );
	GameAssert( status == FILE_SUCCESS );

	data[size] = 0;

	// Close file
	status = File::close( file );
	GameAssert( status == FILE_SUCCESS );

	return data;
}







Material::Material()
{
	memset( this, 0, sizeof( Material ) );
}

void Material::Destroy()
{
	if( this->inputLayout != nullptr )
	{
		this->inputLayout->Release();
	}

	if( this->pixelShader != nullptr )
	{
		this->pixelShader->Release();
	}

	if( this->pixelShaderBuffers != nullptr )
	{
		for( unsigned int i = 0; i < this->pixelShaderBufferCount; i++ )
		{
			this->pixelShaderBuffers[i]->Release();
		}

		delete this->pixelShaderBuffers;
	}

	if( this->vertexShader != nullptr )
	{
		this->vertexShader->Release();
	}
	
	if( this->vertexShaderBuffers != nullptr )
	{
		for( unsigned int i = 0; i < this->vertexShaderBufferCount; i++ )
		{
			this->vertexShaderBuffers[i]->Release();
		}

		delete this->vertexShaderBuffers;
	}
	
	if( this->pixelShaderSamplers != nullptr )
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
	vertexShaderBuffers( nullptr ),
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
	char* vertexShaderData = Load_File( file, size );
	GameCheckFatal( SUCCEEDED( device->CreateVertexShader( vertexShaderData, size, nullptr, &this->vertexShader ) ), "Could not load compiled vertex shader." );

	SetupInputLayout( device, vertexShaderData, size );
	delete vertexShaderData;

	strcpy_s( file, shaderFileBase );
	strcat_s( file, ".ps.cso" );

	char* pixelShaderData = Load_File( file, size );
	GameCheckFatal( SUCCEEDED( device->CreatePixelShader( pixelShaderData, size, nullptr, &this->pixelShader ) ), "Could not load compiled pixel shader." );
	delete pixelShaderData;
}

#define INPUT_LAYOUT_DESC_COUNT 3
void Material::SetupInputLayout( ID3D11Device* device, const char* vertexShaderData, unsigned long dataSize )
{
	D3D11_INPUT_ELEMENT_DESC layout[INPUT_LAYOUT_DESC_COUNT] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	GameCheckFatal( SUCCEEDED( device->CreateInputLayout( layout, INPUT_LAYOUT_DESC_COUNT, vertexShaderData, dataSize, &this->inputLayout ) ), "Could not create vertex shader input layout." );
}

void Material::Draw( const DrawInfo& info ) const
{
	info.context->IASetInputLayout( this->inputLayout );
	info.context->VSSetShader( this->vertexShader, nullptr, 0 );
	info.context->PSSetShader( this->pixelShader, nullptr, 0 );

	this->PrepareBuffers( info );

	info.context->VSSetConstantBuffers( 0, this->vertexShaderBufferCount, this->vertexShaderBuffers );
	info.context->PSSetConstantBuffers( 0, this->pixelShaderBufferCount, this->pixelShaderBuffers );
	info.context->PSSetSamplers( 0, this->pixelShaderSamplerCount, this->pixelShaderSamplers );

	// TODO This will change when skinning is finished
	info.model->Draw( info );
}