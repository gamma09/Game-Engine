#include <MemorySetup.h>
#include <SceneAsset.h>
#include "ModelObject.h"


ModelObject::ModelObject( CLI::Browser* inBrowser, SceneAsset* scene, const char* archiveFile, ID3D11Device* device )
	: CLI::ContentObject(),
	browser( inBrowser )
{
	GameAssert( scene != nullptr );
	GameAssert( archiveFile != nullptr );

	this->asset = scene->AddModel( device, archiveFile );
	this->asset->SetDeleteListener( this );
	
	this->SetName( this->asset->name );
	this->browser->AddModel( this );
}

ModelObject::~ModelObject()
{
	// Do nothing -> scene asset handles model asset destruction
}

ModelAsset* ModelObject::GetModelAsset() const
{
	return this->asset;
}

void ModelObject::AssetDeleted( ModelAsset* model )
{
	GameAssert( model == this->asset );

	this->browser->RemoveModel( this );

	delete this;
}