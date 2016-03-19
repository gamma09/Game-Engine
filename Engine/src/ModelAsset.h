#pragma once

#include <Reflection.h>
#include "Asset.h"

class ModelBase;
struct ID3D11Device;

class ModelAsset : public Asset<ModelAsset>
{
	REFLECTED_CLASS( ModelAsset )

public:
	// This is the reflection constructor
	ModelAsset();

	// This is the specialization constructor (used to create new ModelAssets in the editor)
	ModelAsset( ID3D11Device* device, const char* archiveFile, const char* name );

	ModelAsset( const ModelAsset& asset );
	ModelAsset& operator=( const ModelAsset& asset );

	// Assets should not be deleted directly - instead, you should call DeleteAsset()
	virtual ~ModelAsset() override;

	ModelBase* GetModelBase() const;

	bool operator==( const ModelAsset& model ) const;

	


public:
	REFLECTED( char*, file );
	REFLECTED( char*, name );

private:
	// TODO set a custom serialization function for this pointer
	ModelAsset* next;
	ModelBase* modelBase;


	friend class SceneAsset;
};