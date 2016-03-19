#pragma once

#include <stdint.h>
#include <Reflection.h>
#include "Asset.h"
#include "ModelAsset.h"
#include "DrawInfo.h"

class Actor;
class Material;
class UpdateStrategy;
struct ID3D11Device;

class ActorAsset : public Asset<ActorAsset>
{
	REFLECTED_CLASS( ActorAsset );
public:
	ActorAsset();
	ActorAsset( const char* name, const ModelAsset& model, const Material* material, UpdateStrategy* updateStrategy );

	// Assets should not be deleted directly - call DeleteAsset() instead
	virtual ~ActorAsset() override;
	Actor* GetActor() const;

	bool operator==( const ActorAsset& actor ) const;

	void Update( uint32_t totalTimeMillis ) const;
	void Draw( DrawInfo& info ) const;


private:
	ActorAsset( const ActorAsset& asset ) = delete;
	ActorAsset& operator=( const ActorAsset& asset ) = delete;


public:
	REFLECTED( char*, name );
	REFLECTED( ModelAsset, model );
	REFLECTED( float, x );
	REFLECTED( float, y );
	REFLECTED( float, z );
	REFLECTED( float, rx );
	REFLECTED( float, ry );
	REFLECTED( float, rz );
	REFLECTED( float, sx );
	REFLECTED( float, sy );
	REFLECTED( float, sz );


	// TODO make this pointer reflected
private:
	ActorAsset* next;
	Actor* actor;

	friend class SceneAsset;
};