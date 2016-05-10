#include <d3d11_1.h>
#include <stdio.h>
#include <stdlib.h>

#include <GameAssert.h>
#include <DirectXTex.h>
#include <asset_reader.h>

#include "DirectXAssert.h"
#include "MemorySetup.h"
#include "Texture.h"
#include "TextureManager.h"


// If something has UV values outside [0, 1], it will be textured bright green
static const float BORDER_COLOR[] ={ 0.0f, 1.0f, 0.0f, 1.0f };


void Texture::Set( ID3D11Device* device, const char* archiveFile, const char* textureName )
{
	unsigned char* bytes;
	int size;

	DirectX::TexMetadata metadata;
	DirectX::ScratchImage image;

	if( !read_asset( archiveFile, TEXTURE_TYPE, textureName, bytes, size, TemporaryHeap::Instance() ) )
	{
		this->textureResource = TextureManager::Instance()->Default_Texture()->Get_Texture_Resource();
		this->using_default_texture = true;
		return;
	}
	else if( FAILED( DirectX::LoadFromTGAMemory( bytes, size, &metadata, image ) ) )
	{
		this->using_default_texture = true;
		this->textureResource = TextureManager::Instance()->Default_Texture()->Get_Texture_Resource();
	}
	else if( FAILED( DirectX::CreateShaderResourceView( device, image.GetImages(), image.GetImageCount(), metadata, &this->textureResource ) ) )
	{
		this->using_default_texture = true;
		this->textureResource = TextureManager::Instance()->Default_Texture()->Get_Texture_Resource();
	}
	else
	{
		this->using_default_texture = false;
	}

	delete[] bytes;
}

void Texture::Reset()
{
	ManagedObject::Reset();

	this->nextTexture = 0;

	if( !this->using_default_texture )
	{
		this->textureResource->Release();
		this->textureResource = nullptr;
	}
}


Texture::Texture( ID3D11Device* device, const char* defaultArchive, const char* textureName )
	: ManagedObject(),
	using_default_texture( false ),
	nextTexture( nullptr ),
	textureResource( nullptr )
{
	unsigned char* bytes;
	int size;
	DirectX::TexMetadata metadata;
	DirectX::ScratchImage image;

	GameCheckFatal( read_asset( defaultArchive, TEXTURE_TYPE, textureName, bytes, size, TemporaryHeap::Instance() ), "Error: Could not load default texture from archive." );
	GameCheckFatalDx(  DirectX::LoadFromTGAMemory( bytes, size, &metadata, image ), "Error: Could not read default texture's image data." );
	GameCheckFatalDx(  DirectX::CreateShaderResourceView( device, image.GetImages(), image.GetImageCount(), metadata, &this->textureResource ), "Error: Could not create default texture's shader resource view." );
}

Texture::Texture()
	: ManagedObject(),
	using_default_texture( false ),
	nextTexture( nullptr ),
	textureResource( nullptr )
{
	// Do nothing
}

Texture::~Texture()
{
	if( this->textureResource != nullptr )
	{
		this->textureResource->Release();
	}
}