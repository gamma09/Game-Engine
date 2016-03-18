#pragma once

#include <SceneAsset.h>
#include <Browser.h>

class ModelObject;
class ActorObject;

struct ID3D11Device;

class Scene
{
public:
	Scene( CLI::Browser* browser, const char* name );
	~Scene();

	ModelObject* AddModel( ID3D11Device* device, const char* archiveFile );
	ActorObject* AddActor( ID3D11Device* device, const ModelObject& model, const Material* material, UpdateStrategy* updateStrategy );
	void RemoveActor( ActorObject* actor );

	void Update( uint32_t totalTimeMillis ) const;
	void Draw( DrawInfo& info ) const;

private:
	Scene( const Scene& scene ) = delete;
	Scene& operator=( const Scene& scene ) = delete;


private:
	SceneAsset* asset;
	ModelObject* modelsHead;
	ActorObject* actorsHead;

	CLI::Browser* browser;
};