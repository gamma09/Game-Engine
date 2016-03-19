#pragma once

#include <ContentObject.h>
#include <Browser.h>
#include <ModelAsset.h>
#include <AssetDeleteListener.h>

class SceneAsset;
struct ID3D11Device;

class ModelObject : public CLI::ContentObject, public AssetDeleteListener<ModelAsset>
{
public:
	ModelObject( CLI::Browser* browser, SceneAsset* scene, const char* archiveFile, ID3D11Device* device );
	virtual ~ModelObject() override;

	ModelAsset* GetModelAsset() const;

	virtual void AssetDeleted( ModelAsset* model ) override;

private:
	ModelObject( const ModelObject& obj ) = delete;
	ModelObject& operator=( const ModelObject& obj ) = delete;

private:
	ModelAsset* asset;

	CLI::Browser* browser;

	friend class Scene;
};