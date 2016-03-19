#include "ModelAsset.h"
#include "ModelBase.h"
#include "ModelBaseManager.h"
#include "MemorySetup.h"

ModelAsset::ModelAsset()
	: Asset(),
	file( nullptr ),
	name( nullptr ),
	modelBase( nullptr ),
	next( nullptr )
{
	// Do nothing
}


ModelAsset::ModelAsset( ID3D11Device* device, const char* archiveFile, const char* name )
	: next( nullptr )
{
	GameAssert( archiveFile != nullptr );
	GameAssert( name != nullptr );
	GameAssert( device != nullptr );

	size_t fileStrSize = strlen( archiveFile ) + 1;
	this->file = newArray( char, fileStrSize, AssetHeap::Instance(), ALIGN_4 );
	strcpy_s( this->file, fileStrSize, archiveFile );
	
	size_t nameSize = strlen( name ) + 1;
	this->name = newArray( char, nameSize, AssetHeap::Instance(), ALIGN_4 );
	strcpy_s( this->name, nameSize, name );

	this->modelBase = ModelBaseManager::Instance()->Add( device, this->file );
	this->modelBase->Add_Reference();
}

ModelAsset::ModelAsset( const ModelAsset& asset )
	: modelBase( asset.modelBase ),
	next( asset.next )
{
	if( modelBase != nullptr )
	{
		modelBase->Add_Reference();
	}

	GameAssert( asset.file != nullptr );
	GameAssert( asset.name != nullptr );

	size_t fileStrSize = strlen( asset.file ) + 1;
	this->file = newArray( char, fileStrSize, AssetHeap::Instance(), ALIGN_4 );
	strcpy_s( this->file, fileStrSize, asset.file );

	size_t nameSize = strlen( asset.name ) + 1;
	this->name = newArray( char, nameSize, AssetHeap::Instance(), ALIGN_4 );
	strcpy_s( this->name, nameSize, asset.name );
}

ModelAsset& ModelAsset::operator=( const ModelAsset& asset )
{
	this->next = asset.next;

	ModelBase* oldModelBase = this->modelBase;
	this->modelBase = asset.modelBase;
	this->modelBase->Add_Reference();

	if( oldModelBase != nullptr )
	{
		oldModelBase->Remove_Reference();
	}

	if( this->file != nullptr )
	{
		delete this->file;
	}

	if( this->name != nullptr )
	{
		delete this->name;
	}

	GameAssert( asset.file != nullptr );
	GameAssert( asset.name != nullptr );

	size_t fileStrSize = strlen( asset.file ) + 1;
	this->file = newArray( char, fileStrSize, AssetHeap::Instance(), ALIGN_4 );
	strcpy_s( this->file, fileStrSize, asset.file );

	size_t nameSize = strlen( asset.name ) + 1;
	this->name = newArray( char, nameSize, AssetHeap::Instance(), ALIGN_4 );
	strcpy_s( this->name, nameSize, asset.name );

	return *this;
}

ModelAsset::~ModelAsset()
{
	if( this->file != nullptr )
	{
		delete this->file;
	}

	if( this->name != nullptr )
	{
		delete this->name;
	}

	if( this->modelBase != nullptr )
	{
		this->modelBase->Remove_Reference();
	}
}

ModelBase* ModelAsset::GetModelBase() const
{
	return this->modelBase;
}

bool ModelAsset::operator==( const ModelAsset& model ) const
{
	return strcmp( this->file, model.file ) == 0;
}