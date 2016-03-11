#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <Windows.h>
#include <d3d11_1.h>

#include <File.h>
#include <GameAssert.h>
#include <MathEngine.h>

#include "MemorySetup.h"
#include "DebuggerSetup.h"

#include "Shader.h"
#include "ShaderManager.h"



static char* Load_File( const char* fileName, unsigned long& size );



Shader::Shader()
	: ManagedObject(),
	ReferencedObject(),
	name( Not_Initialized ),
	vertexShader( nullptr ),
	vertexShaderBufferCount( 0 ),
	vertexShaderBuffers( nullptr ),
	pixelShader( nullptr ),
	pixelShaderBufferCount( 0 ),
	pixelShaderBuffers( nullptr ),
	inputLayout( nullptr ),
	pixelShaderSamplerCount( 0 ),
	pixelShaderSamplers( nullptr )
{
	// Do nothing
}

Shader::~Shader()
{
	// Do nothing
}

void Shader::Set( const Name& name, const char* shaderFileAsset, ID3D11Device* device )
{
	GameAssert( shaderFileAsset != 0 );
	GameAssert( name != Name::Not_Initialized );

	this->name = name;
	char file[256];

	strcpy_s( file, shaderFileAsset );
	strcat_s( file, ".vs.cso" );

	unsigned long size;
	char* vertexShaderData = Load_File( file, size );
	GameCheckFatal( SUCCEEDED( device->CreateVertexShader( vertexShaderData, size, nullptr, &this->vertexShader ) ), "Could not load compiled vertex shader." );
	
	SetupInputLayout( name, device, vertexShaderData, size );
	delete vertexShaderData;

	SetupVertexShaderBuffers( name, device );

	strcpy_s( file, shaderFileAsset );
	strcat_s( file, ".ps.cso" );

	char* pixelShaderData = Load_File( file, size );
	GameCheckFatal( SUCCEEDED( device->CreatePixelShader( pixelShaderData, size, nullptr, &this->pixelShader ) ), "Could not load compiled pixel shader." );
	delete pixelShaderData;

	SetupPixelShaderBuffers( name, device );
	SetupPixelShaderSamplers( name, device );
}

void Shader::SetupInputLayout( const Name& name, ID3D11Device* device, const char* vertexShaderData, unsigned long dataSize )
{
	D3D11_INPUT_ELEMENT_DESC* layout;
	unsigned int layoutCount;

	switch( name )
	{
		case TEXTURE_LIGHT:
			layoutCount = 3;
			layout = new( TemporaryHeap::Instance(), ALIGN_4 ) D3D11_INPUT_ELEMENT_DESC[3];
			layout[0] ={ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
			layout[1] ={ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 };
			layout[2] ={ "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 };
			break;

		case TEXTURE:
			layoutCount = 2;
			layout = new( TemporaryHeap::Instance(), ALIGN_4 ) D3D11_INPUT_ELEMENT_DESC[2];
			layout[0] ={ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
			layout[1] ={ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 };
			break;

		case WIREFRAME:
			layoutCount = 1;
			layout = new( TemporaryHeap::Instance(), ALIGN_4 ) D3D11_INPUT_ELEMENT_DESC[1];
			layout[0] ={ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
			break;

		default:
			GameAssert( 0 );
			break;
	}

	GameCheckFatal( SUCCEEDED( device->CreateInputLayout( layout, layoutCount, vertexShaderData, dataSize, &this->inputLayout ) ), "Could not create vertex shader input layout." );
	delete layout;
}

void Shader::SetupVertexShaderBuffers( const Name& name, ID3D11Device* device )
{
	switch( name )
	{
		case TEXTURE_LIGHT:
			this->vertexShaderBufferCount = 4;
			break;

		case TEXTURE:
			this->vertexShaderBufferCount = 3;
			break;

		case WIREFRAME:
			this->vertexShaderBufferCount = 3;
			break;

		default:
			GameAssert( 0 );
			break;
	}

	D3D11_BUFFER_DESC* bd = new( TemporaryHeap::Instance(), ALIGN_4 ) D3D11_BUFFER_DESC[this->vertexShaderBufferCount];
	for( unsigned int i = 0; i < this->vertexShaderBufferCount; i++ )
	{
		bd[i].Usage = D3D11_USAGE_DEFAULT;
		bd[i].BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd[i].CPUAccessFlags = 0;
		bd[i].MiscFlags = 0;
		bd[i].StructureByteStride = 0;
	}

	switch( name )
	{
		case TEXTURE_LIGHT:
			bd[0].ByteWidth = sizeof( Matrix );
			bd[1].ByteWidth = sizeof( Matrix );
			bd[2].ByteWidth = sizeof( Matrix );
			bd[3].ByteWidth = sizeof( Vect ) + sizeof( Vect );
			break;

		case TEXTURE:
			bd[0].ByteWidth = sizeof( Matrix );
			bd[1].ByteWidth = sizeof( Matrix );
			bd[2].ByteWidth = sizeof( Matrix );
			break;

		case WIREFRAME:
			bd[0].ByteWidth = sizeof( Matrix );
			bd[1].ByteWidth = sizeof( Matrix );
			bd[2].ByteWidth = sizeof( Matrix );
			break;

		default:
			GameAssert( 0 );
			break;
	}

	this->vertexShaderBuffers = new( ConstantBufferHeap::Instance(), ALIGN_4 ) ID3D11Buffer*[this->vertexShaderBufferCount];

	for( unsigned int i = 0; i < this->vertexShaderBufferCount; i++ )
	{
		GameCheckFatal( SUCCEEDED( device->CreateBuffer( &bd[i], nullptr, &this->vertexShaderBuffers[i] ) ), "Could not create vertex shader's constant buffers." );
	}

	delete bd;
}

void Shader::SetupPixelShaderBuffers( const Name& name, ID3D11Device* device )
{
	switch( name )
	{
		case TEXTURE_LIGHT:
			this->pixelShaderBufferCount = 0;
			break;

		case TEXTURE:
			this->pixelShaderBufferCount = 0;
			break;

		case WIREFRAME:
			this->pixelShaderBufferCount = 0;
			break;

		default:
			GameAssert( 0 );
			break;
	}

	D3D11_BUFFER_DESC* bd = new( TemporaryHeap::Instance(), ALIGN_4 ) D3D11_BUFFER_DESC[this->pixelShaderBufferCount];
	for( unsigned int i = 0; i < this->pixelShaderBufferCount; i++ )
	{
		bd[i].Usage = D3D11_USAGE_DEFAULT;
		bd[i].BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd[i].CPUAccessFlags = 0;
		bd[i].MiscFlags = 0;
		bd[i].StructureByteStride = 0;
	}

	switch( name )
	{
		case TEXTURE_LIGHT:
			break;

		case TEXTURE:
			break;

		case WIREFRAME:
			break;

		default:
			GameAssert( 0 );
			break;
	}

	this->pixelShaderBuffers = new( ConstantBufferHeap::Instance(), ALIGN_4 ) ID3D11Buffer*[this->pixelShaderBufferCount];

	for( unsigned int i = 0; i < this->pixelShaderBufferCount; i++ )
	{
		GameCheckFatal( SUCCEEDED( device->CreateBuffer( &bd[i], nullptr, &this->pixelShaderBuffers[i] ) ), "Could not create pixel shader's constant buffers." );
	}

	delete bd;
}

void Shader::SetupPixelShaderSamplers( const Name& name, ID3D11Device* device )
{
	switch( name )
	{
		case TEXTURE_LIGHT:
			this->pixelShaderSamplerCount = 1;
			break;

		case TEXTURE:
			this->pixelShaderSamplerCount = 1;
			break;

		case WIREFRAME:
			this->pixelShaderSamplerCount = 0;
			break;

		default:
			GameAssert( 0 );
			break;
	}

	D3D11_SAMPLER_DESC* sd = new( TemporaryHeap::Instance(), ALIGN_4 ) D3D11_SAMPLER_DESC[this->pixelShaderSamplerCount];
	for( unsigned int i = 0; i < this->pixelShaderSamplerCount; i++ )
	{
		sd[i].Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sd[i].AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sd[i].AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sd[i].AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sd[i].MipLODBias = 0;
		sd[i].MaxAnisotropy = 0;
		sd[i].ComparisonFunc = D3D11_COMPARISON_NEVER;
		sd[i].BorderColor[0] = 0.0f;
		sd[i].BorderColor[1] = 0.0f;
		sd[i].BorderColor[2] = 0.0f;
		sd[i].BorderColor[3] = 1.0f;
		sd[i].MinLOD = 0;
		sd[i].MaxLOD = D3D11_FLOAT32_MAX;
	}

	switch( name )
	{
		case TEXTURE_LIGHT:
			break;

		case TEXTURE:
			break;

		case WIREFRAME:
			break;

		default:
			GameAssert( 0 );
			break;
	}

	this->pixelShaderSamplers = new( ConstantBufferHeap::Instance(), ALIGN_4 ) ID3D11SamplerState*[this->pixelShaderSamplerCount];

	for( unsigned int i = 0; i < this->pixelShaderSamplerCount; i++ )
	{
		GameCheckFatal( SUCCEEDED( device->CreateSamplerState( &sd[i], &this->pixelShaderSamplers[i] ) ), "Could not create pixel shader's texture samplers." );
	}

	delete sd;
}

void Shader::Reset()
{
	ManagedObject::Reset();

	GameAssert( this->Get_Reference_Count() == 0 );

	this->inputLayout->Release();
	this->inputLayout = nullptr;

	this->pixelShader->Release();
	this->pixelShader = nullptr;

	for( unsigned int i = 0; i < this->pixelShaderBufferCount; i++ )
	{
		this->pixelShaderBuffers[i]->Release();
	}

	delete this->pixelShaderBuffers;
	this->pixelShaderBuffers = nullptr;
	this->pixelShaderBufferCount = 0;

	this->vertexShader->Release();
	this->vertexShader = nullptr;

	for( unsigned int i = 0; i < this->vertexShaderBufferCount; i++ )
	{
		this->vertexShaderBuffers[i]->Release();
	}

	delete this->vertexShaderBuffers;
	this->vertexShaderBuffers = nullptr;
	this->vertexShaderBufferCount = 0;

	for( unsigned int i = 0; i < this->pixelShaderSamplerCount; i++ )
	{
		this->pixelShaderSamplers[i]->Release();
	}

	delete this->pixelShaderSamplers;
	this->pixelShaderSamplers = nullptr;
	this->pixelShaderSamplerCount = 0;

	this->name = Name::Not_Initialized;
}

void Shader::Free_Me()
{
	ShaderManager::Instance()->Remove( this );
}

void Shader::Activate( ID3D11DeviceContext* context ) const
{
	GameAssert( this->name != Name::Not_Initialized );
	context->IASetInputLayout( this->inputLayout );

	context->VSSetShader( this->vertexShader, nullptr, 0 );
	context->VSSetConstantBuffers( 0, this->vertexShaderBufferCount, this->vertexShaderBuffers );

	context->PSSetShader( this->pixelShader, nullptr, 0 );
	context->PSSetConstantBuffers( 0, this->pixelShaderBufferCount, this->pixelShaderBuffers );
}

ID3D11Buffer* Shader::GetVertexShaderBuffer( unsigned int index ) const
{
	GameAssert( this->name != Name::Not_Initialized );
	GameAssert( index < this->vertexShaderBufferCount );
	return this->vertexShaderBuffers[index];
}

ID3D11Buffer* Shader::GetPixelShaderBuffer( unsigned int index ) const
{
	GameAssert( this->name != Name::Not_Initialized );
	GameAssert( index < this->pixelShaderBufferCount );
	return this->pixelShaderBuffers[index];
}





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