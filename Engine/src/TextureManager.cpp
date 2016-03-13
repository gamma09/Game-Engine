#include <GameAssert.h>


#include "TextureManager.h"
#include "Texture.h"
#include "MemorySetup.h"

#define MAX_TEXTURES_CREATED 128


TextureManager* TextureManager::instance;

TextureManager* TextureManager::Instance()
{
	GameAssert( instance != 0 );

	return instance;
}

void TextureManager::Create( Heap* managerHeap, const uint32_t initialReserve, const uint32_t refillSize )
{
	managerHeap;

	GameAssert( instance == 0 );

	instance = new( managerHeap, ALIGN_4 ) TextureManager( initialReserve, refillSize );
}

void TextureManager::Destroy()
{
	GameAssert( instance != 0 );

	delete instance;
	instance = 0;
}

Texture* TextureManager::Add( ID3D11Device* device, const char* archiveFile, const char* textureName )
{
	Texture* texture = static_cast<Texture*>( this->Add_Object() );
	texture->Set( device, archiveFile, textureName );
	return texture;
}

const Texture* TextureManager::Default_Texture() const
{
	GameAssert( this->defaultTexture != 0 );

	return this->defaultTexture;
}

void TextureManager::Create_Default_Texture( ID3D11Device* device )
{
	GameAssert( this->defaultTexture == 0 );
	this->defaultTexture = new( this->heap, ALIGN_4 ) Texture( device, "../resources/Default_Texture.spu", "default" );
}


ManagedObject* TextureManager::Make_Object() const
{
	return new( this->heap, ALIGN_4 ) Texture();
}

void TextureManager::Delete_Object( ManagedObject* obj ) const
{
	delete obj;
}


TextureManager::TextureManager( const uint32_t initialReserve, const uint32_t refillSize )
	: Manager( refillSize ),
	defaultTexture( nullptr )
{
	Mem::createFixBlockHeap( this->heap, MAX_TEXTURES_CREATED, sizeof( Texture ) );
	this->Init( initialReserve );
}

TextureManager::~TextureManager()
{
	delete this->defaultTexture;
	Destroy_Objects();
	Mem::destroyHeap( heap );
}