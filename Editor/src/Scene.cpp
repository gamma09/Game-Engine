#include <d3d11_1.h>
#include <MemorySetup.h>
#include <ModelAsset.h>
#include "Scene.h"
#include "ModelObject.h"
#include "ActorObject.h"

Scene::Scene( CLI::Browser* browser, const char* name )
	: asset( new( AssetHeap::Instance(), ALIGN_4 ) SceneAsset( name ) ),
	modelsHead( nullptr ),
	actorsHead( nullptr ),
	browser( browser )
{
	// Do nothing
}

Scene::~Scene()
{
	while( this->actorsHead != nullptr )
	{
		ActorObject* actor = this->actorsHead;
		this->actorsHead = this->actorsHead->next;

		delete actor;
	}

	while( this->modelsHead != nullptr )
	{
		ModelObject* model = this->modelsHead;
		this->modelsHead = this->modelsHead->next;

		delete model;
	}

	delete asset;
}

ModelObject* Scene::AddModel( ID3D11Device* device, const char* archiveFile )
{
	// Add the model
	ModelObject* obj = new( AssetHeap::Instance(), ALIGN_4 ) ModelObject( this->asset, archiveFile );
	
	// Set up the model base
	obj->GetModelAsset()->SetupModelBase( device );

	if( this->modelsHead != nullptr )
	{
		this->modelsHead->prev = obj;
	}

	obj->next = this->modelsHead;
	obj->prev = nullptr;
	
	this->modelsHead = obj;

	this->browser->AddModel( obj );

	return obj;
}

ActorObject* Scene::AddActor( ID3D11Device* device, const ModelObject& model, const Material* material, UpdateStrategy* updateStrategy )
{
	GameAssert( material != nullptr );
	GameAssert( updateStrategy != nullptr );

	this->asset->LockScene();

	// Add the actor
	ActorObject* obj = new( AssetHeap::Instance(), ALIGN_4 ) ActorObject( this->asset, *model.GetModelAsset(), material, updateStrategy );

	// Set up the actor
	obj->GetActorAsset()->SetupActor( device );

	if( this->actorsHead != nullptr )
	{
		this->actorsHead->prev = obj;
	}

	obj->next = this->actorsHead;
	obj->prev = nullptr;

	this->actorsHead = obj;

	this->asset->UnlockScene();

	this->browser->AddActor( obj );

	return obj;
}

void Scene::RemoveActor( ActorObject* actor )
{
	GameAssert( actor != nullptr );

	this->asset->LockScene();

	if( actor->prev == nullptr )
	{
		GameAssert( this->actorsHead == actor );
		this->actorsHead = actor->next;
	}
	else
	{
		actor->prev->next = actor->next;
	}

	if( actor->next != nullptr )
	{
		actor->next->prev = actor->prev;
	}

	this->browser->RemoveActor( actor );

	delete actor;

	this->asset->UnlockScene();
}

void Scene::Update( uint32_t totalTimeMillis ) const
{
	this->asset->Update( totalTimeMillis );
}

void Scene::Draw( DrawInfo& info ) const
{
	this->asset->Draw( info );
}
