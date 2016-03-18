#include <thread>

#include "SceneAsset.h"
#include "MemorySetup.h"
#include "ActorAsset.h"
#include "ModelAsset.h"

SceneAsset::SceneAsset()
	: name( nullptr ),
	modelsHead( nullptr ),
	actorsHead( nullptr ),
	listLock(),
	lockCount( 0 ),
	lockOwner( 0 )
{
	this->listLock.clear();
}

SceneAsset::SceneAsset( const char* name )
	: actorsHead( nullptr ),
	modelsHead( nullptr ),
	listLock(),
	lockCount( 0 ),
	lockOwner( 0 )
{
	this->listLock.clear();

	GameAssert( name != nullptr );

	size_t size = strlen( name ) + 1;
	this->name = newArray( char, size, AssetHeap::Instance(), ALIGN_4 );
	strcpy_s( this->name, size, name );
}


SceneAsset::~SceneAsset()
{
	while( this->actorsHead != nullptr )
	{
		ActorAsset* actor = this->actorsHead;
		this->actorsHead = this->actorsHead->next;

		delete actor;
	}

	while( this->modelsHead != nullptr )
	{
		ModelAsset* model = this->modelsHead;
		this->modelsHead = this->modelsHead->next;

		delete model;
	}

	delete this->name;
}

ModelAsset* SceneAsset::AddModel( const char* archiveFile )
{
	size_t size = strlen( archiveFile ) + 1;
	char* pathname = newArray( char, size, TemporaryHeap::Instance(), ALIGN_4 );
	strcpy_s( pathname, size, archiveFile );

	char* nameA = strrchr( pathname, '/' );
	char* nameB = strrchr( pathname, '\\' );

	char* name;
	if( nameA == nullptr && nameB == nullptr )
	{
		name = pathname;
	}
	else if( nameA == nullptr )
	{
		name = nameB + 1;
	}
	else if( nameB == nullptr )
	{
		name = nameA + 1;
	}
	else
	{
		name = max( nameA + 1, nameB + 1 );
	}

	*( strchr( name, '.' ) ) = 0;

	ModelAsset* asset = new( AssetHeap::Instance(), ALIGN_4 ) ModelAsset( archiveFile, name );
	GameAssert( !this->Exists( name ) );

	delete pathname;

	this->LockScene();

	asset->next = this->modelsHead;

	std::atomic_thread_fence( std::memory_order_release );

	this->modelsHead = asset;

	this->UnlockScene();

	return asset;
}

ActorAsset* SceneAsset::AddActor( const ModelAsset& model, const Material* material, UpdateStrategy* updateStrategy )
{
	GameAssert( this->Contains( model ) );

	size_t actorNameSize = strlen( model.name ) + 1 + 11;
	char* actorName = newArray( char, actorNameSize, TemporaryHeap::Instance(), ALIGN_4 );
	strcpy_s( actorName, actorNameSize, "Actor_" );
	strcat_s( actorName, actorNameSize, model.name );
	strcat_s( actorName, actorNameSize, "_" );

	size_t numberSize = actorNameSize - strlen( actorName );
	char* numberStart = strchr( actorName, 0 );

	this->LockScene();

	// For now, we only allow up to 10000 instances of a particular model in the scene -> to increase this, you must also increase the actorNameSize
	for( unsigned int i = 0; i < 10000; i++ )
	{
		sprintf_s( numberStart, numberSize, "%u", i );
		if( !this->ExistsInLock( actorName ) )
		{
			break;
		}
	}

	// Make sure we didn't hit the 10000 instance cap
	GameAssert( !this->ExistsInLock( actorName ) );

	ActorAsset* asset = new( AssetHeap::Instance(), ALIGN_4 ) ActorAsset( actorName, model, material, updateStrategy );

	delete actorName;

	asset->next = this->actorsHead;
	this->actorsHead = asset;

	this->UnlockScene();

	return asset;
}

void SceneAsset::RemoveActor( const ActorAsset& actor )
{
	this->LockScene();

	ActorAsset* prev = nullptr;
	ActorAsset* curr = this->actorsHead;
	while( curr != nullptr )
	{
		if( *curr == actor )
		{
			break;
		}

		prev = curr;
		curr = curr->next;
	}
	
	GameAssert( curr != nullptr );

	if( prev == nullptr )
	{
		this->actorsHead = curr->next;
	}
	else
	{
		prev->next = curr->next;
	}

	this->UnlockScene();

	delete curr;
}

bool SceneAsset::Contains( const ModelAsset& model ) const
{
	// We don't need to lock the list for this - the memory fence in AddModel makes this safe
	for( const ModelAsset* curr = this->modelsHead; curr != nullptr; curr = curr->next )
	{
		if( *curr == model )
		{
			return true;
		}
	}

	return false;
}

bool SceneAsset::Contains( const ActorAsset& actor )
{
	bool retVal = false;

	this->LockScene();

	for( const ActorAsset* curr = this->actorsHead; curr != nullptr; curr = curr->next )
	{
		if( *curr == actor )
		{
			retVal = true;
		}
	}
	
	this->UnlockScene();

	return retVal;
}

unsigned int SceneAsset::GetUsageCount( const ModelAsset& model )
{
	unsigned int usageCount = 0;

	this->LockScene();

	for( const ActorAsset* curr = this->actorsHead; curr != nullptr; curr = curr->next )
	{
		usageCount += ( curr->model == model ) ? 1 : 0;
	}

	this->UnlockScene();

	return usageCount;
}

bool SceneAsset::Exists( const char* assetName )
{
	this->LockScene();

	bool exists = this->ExistsInLock( assetName );

	this->UnlockScene();

	return exists;
}

bool SceneAsset::ExistsInLock( const char* assetName ) const
{
	for( const ActorAsset* curr = this->actorsHead; curr != nullptr; curr = curr->next )
	{
		if( strcmp( curr->name, assetName ) == 0 )
		{
			return true;
		}
	}

	for( const ModelAsset* curr = this->modelsHead; curr != nullptr; curr = curr->next )
	{
		if( strcmp( curr->name, assetName ) == 0 )
		{
			return true;
		}
	}

	return false;
}

void SceneAsset::Update( uint32_t totalTimeMillis )
{
	this->LockScene();

	for( const ActorAsset* actor = this->actorsHead; actor != nullptr; actor = actor->next )
	{
		actor->Update( totalTimeMillis );
	}

	this->UnlockScene();
}

void SceneAsset::Draw( DrawInfo& info )
{
	this->LockScene();

	for( const ActorAsset* actor = this->actorsHead; actor != nullptr; actor = actor->next )
	{
		actor->Draw( info );
	}

	this->UnlockScene();
}

void SceneAsset::LockScene()
{
	// Lock the lists
	while( this->listLock.test_and_set( std::memory_order_acq_rel ) )
	{
		if( this->lockOwner == GetCurrentThreadId() )
		{
			break;
		}
		std::this_thread::yield();
	}

	this->lockOwner = GetCurrentThreadId();
	this->lockCount += 1;
}

void SceneAsset::UnlockScene()
{
	GameAssert( GetCurrentThreadId() == this->lockOwner );

	this->lockCount -= 1;
	if( this->lockCount == 0 )
	{
		this->lockOwner = 0;
		this->listLock.clear( std::memory_order_release );
	}
}
