#pragma once

#include <ContentObject.h>
#include <ActorAsset.h>

class SceneAsset;

class ActorObject : public CLI::ContentObject
{
public:
	ActorObject( SceneAsset* scene, const ModelAsset& model, const Material* material, UpdateStrategy* updateStrategy );
	virtual ~ActorObject() override;

	ActorAsset* GetActorAsset() const;

private:
	ActorObject( const ActorObject& obj ) = delete;
	ActorObject& operator=( const ActorObject& obj ) = delete;



private:
	ActorAsset* asset;
	ActorObject* next;
	ActorObject* prev;
	SceneAsset* scene;


	friend class Scene;
};