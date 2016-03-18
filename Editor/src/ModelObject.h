#pragma once

#include <ContentObject.h>
#include <ModelAsset.h>

class SceneAsset;

class ModelObject : public CLI::ContentObject
{
public:
	ModelObject( SceneAsset* scene, const char* archiveFile );
	virtual ~ModelObject() override;

	ModelAsset* GetModelAsset() const;


private:
	ModelObject( const ModelObject& obj ) = delete;
	ModelObject& operator=( const ModelObject& obj ) = delete;

private:
	ModelAsset* asset;
	ModelObject* next;
	ModelObject* prev;



	friend class Scene;
};