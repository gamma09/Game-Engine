#include <MemorySetup.h>
#include <SceneAsset.h>

#include "ActorObject.h"



ActorObject::ActorObject( SceneAsset* scene, const ModelAsset& model, const Material* material, UpdateStrategy* updateStrategy )
	: CLI::ContentObject(),
	scene( scene )
{
	GameAssert( scene != nullptr );

	this->asset = scene->AddActor( model, material, updateStrategy );
	this->SetName( this->asset->name );
}

ActorObject::~ActorObject()
{
	this->scene->RemoveActor( *this->asset );
}

ActorAsset* ActorObject::GetActorAsset() const
{
	return this->asset;
}
