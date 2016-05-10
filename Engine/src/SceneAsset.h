#pragma once

#include <stdint.h>
#include <Reflection.h>
#include "Asset.h"
#include "YieldMutex.h"

class ModelAsset;
class ActorAsset;
class Material;
class UpdateStrategy;

struct ID3D11Device;

class SceneAsset : public Asset<SceneAsset>
{
	// TODO make this class use a custom serialization function
	REFLECTED_CLASS( SceneAsset );

public:
	SceneAsset();
	SceneAsset( const char* name );

	// SceneAssets should not be deleted directly - instead, you should call DeleteAsset()
	virtual ~SceneAsset() override;

	ModelAsset* AddModel( ID3D11Device* device, const char* archiveName );
	ActorAsset* AddActor( ID3D11Device* device, const ModelAsset& model, const Material* material, UpdateStrategy* updateStrategy );

	void RemoveActor( const ActorAsset& actor );

	bool Contains( const ModelAsset& model ) const;
	bool Contains( const ActorAsset& actor );

	// Returns the number of actors that use the specified model
	unsigned int GetUsageCount( const ModelAsset& model );

	// Returns whether or not the specified name is already being used by another asset
	bool Exists( const char* assetName );

	void Update( uint32_t totalTimeMillis );

	YieldMutex::Lock LockScene();


private:
	bool ExistsInLock( const char* assetName ) const;

	SceneAsset( const SceneAsset& scene ) = delete;
	SceneAsset& operator=( const SceneAsset& scene ) = delete;


private:
	char* name;

	YieldMutex listsMutex;

	ModelAsset* modelsHead;
	ActorAsset* actorsHead;
};