#pragma once

#include <stdint.h>

#include <Manager.h>

class Actor;
class ModelBase;
class Texture;
class Heap;
class UpdateStrategy;
class Material;

struct ID3D11Device;

class ActorManager : public Manager
{
public:
	static ActorManager* Instance();

	static void Create( Heap* managerHeap, uint32_t initialReserve, uint32_t refillSize );
	static void Destroy();

	Actor* Add( ID3D11Device* device, const Material* material, ModelBase* modelBase, UpdateStrategy* updateStrategy );



protected:
	virtual ManagedObject* Make_Object() const override;
	virtual void Delete_Object( ManagedObject* obj ) const override;

private:
	// creation/destruction of manager can only be done by the manager
	ActorManager( uint32_t initialReserve, uint32_t refillSize );
	virtual ~ActorManager();

	// no copying allowed! and no locals either!
	ActorManager();
	ActorManager( const ActorManager& manager );
	ActorManager& operator=( const ActorManager& manager );

private:
	Heap* heap;

	static ActorManager* instance;
};