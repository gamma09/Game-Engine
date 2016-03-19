#include <MemorySetup.h>
#include <SceneAsset.h>

#include "ActorObject.h"



ActorObject::ActorObject( CLI::Browser* inBrowser, SceneAsset* scene, const ModelAsset& model, const Material* material, UpdateStrategy* updateStrategy )
	: CLI::ContentObject(),
	scene( scene ),
	browser( inBrowser )
{
	GameAssert( scene != nullptr );
	GameAssert( inBrowser != nullptr );
	GameAssert( updateStrategy != nullptr );

	{
		YieldMutex::Lock lock = scene->LockScene();
		this->asset = scene->AddActor( model, material, updateStrategy );
		this->asset->SetDeleteListener( this );
	}

	this->SetName( this->asset->name );
	this->browser->AddActor( this );
}

ActorObject::~ActorObject()
{
	// Scene asset handles actor asset destruction
}

ActorAsset* ActorObject::GetActorAsset() const
{
	return this->asset;
}

void ActorObject::AssetDeleted( ActorAsset* actor )
{
	GameAssert( actor == this->asset );
	actor;

	this->browser->RemoveActor( this );

	delete this;
}