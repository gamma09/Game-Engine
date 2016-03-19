#pragma once

#include <ContentObject.h>
#include <ActorAsset.h>
#include <Browser.h>

class SceneAsset;

class ActorObject : public CLI::ContentObject, public AssetDeleteListener<ActorAsset>
{
public:
	ActorObject( CLI::Browser* browser, SceneAsset* scene, const ModelAsset& model, const Material* material, UpdateStrategy* updateStrategy );
	virtual ~ActorObject() override;

	ActorAsset* GetActorAsset() const;

	virtual void AssetDeleted( ActorAsset* actor ) override;

private:
	ActorObject( const ActorObject& obj ) = delete;
	ActorObject& operator=( const ActorObject& obj ) = delete;



private:
	ActorAsset* asset;
	SceneAsset* scene;
	CLI::Browser* browser;
};