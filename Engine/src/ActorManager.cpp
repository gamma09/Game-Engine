#include <GameAssert.h>

#include "MemorySetup.h"
#include "ActorManager.h"
#include "Actor.h"


ActorManager* ActorManager::instance = nullptr;

ActorManager* ActorManager::Instance()
{
	GameAssert( instance );
	return instance;
}

void ActorManager::Create( Heap* managerHeap, uint32_t initialReserve, uint32_t refillSize )
{
	GameAssert( instance == 0 );
	instance = new( managerHeap, ALIGN_4 ) ActorManager( initialReserve, refillSize );
}

void ActorManager::Destroy()
{
	GameAssert( instance );
	instance->PreDestroy();
	delete instance;
	instance = nullptr;
}

Actor* ActorManager::Add( ID3D11Device* device, const Material* material, ModelBase* modelBase, UpdateStrategy* updateStrategy )
{
	Actor* actor = static_cast<Actor*>( this->Add_Object() );
	actor->Set( device, material, modelBase, updateStrategy );
	return actor;
}


ManagedObject* ActorManager::Make_Object() const
{
	return new( this->heap, ALIGN_16 ) Actor();
}

void ActorManager::Delete_Object( ManagedObject* obj ) const
{
	delete obj;
}


ActorManager::ActorManager( uint32_t initialReserve, uint32_t refillSize )
	: Manager( refillSize )
{
	// Hey, might as well allocate a whole page of memory...
	// DO NOT CHANGE TO FIX SIZE HEAP! Actor needs to be 16-byte aligned...
	Mem::createVariableBlockHeap( this->heap, 4096, "Actors" );
	this->Init( initialReserve );
}

ActorManager::~ActorManager()
{
	Mem::destroyHeap( heap );
}