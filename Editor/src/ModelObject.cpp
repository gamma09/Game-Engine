#include <MemorySetup.h>
#include <SceneAsset.h>
#include "ModelObject.h"


ModelObject::ModelObject( SceneAsset* scene, const char* archiveFile )
	: CLI::ContentObject(),
	next( nullptr ),
	prev( nullptr )
{
	GameAssert( scene != nullptr );
	GameAssert( archiveFile != nullptr );

	this->asset = scene->AddModel( archiveFile );
	this->SetName( this->asset->name );
}

ModelObject::~ModelObject()
{
	// Do nothing -> scene asset handles model asset destruction
}

ModelAsset* ModelObject::GetModelAsset() const
{
	return this->asset;
}