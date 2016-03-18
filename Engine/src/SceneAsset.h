#pragma once

#include <stdint.h>
#include <atomic>
#include <Reflection.h>
#include "DrawInfo.h"

class ModelAsset;
class ActorAsset;
class Material;
class UpdateStrategy;

class SceneAsset
{
	// TODO make this class use a custom serialization function
	REFLECTED_CLASS( SceneAsset );

public:
	SceneAsset();
	SceneAsset( const char* name );

	~SceneAsset();

	ModelAsset* AddModel( const char* archiveName );
	ActorAsset* AddActor( const ModelAsset& model, const Material* material, UpdateStrategy* updateStrategy );

	void RemoveActor( const ActorAsset& actor );

	bool Contains( const ModelAsset& model ) const;
	bool Contains( const ActorAsset& actor );

	// Returns the number of actors that use the specified model
	unsigned int GetUsageCount( const ModelAsset& model );

	// Returns whether or not the specified name is already being used by another asset
	bool Exists( const char* assetName );

	void Update( uint32_t totalTimeMillis );
	void Draw( DrawInfo& info );

	void LockScene();
	void UnlockScene();


private:
	bool ExistsInLock( const char* assetName ) const;

	SceneAsset( const SceneAsset& scene ) = delete;
	SceneAsset& operator=( const SceneAsset& scene ) = delete;


private:
	char* name;

	std::atomic_flag listLock;
	DWORD lockOwner;
	DWORD lockCount;

	ModelAsset* modelsHead;
	ActorAsset* actorsHead;
};