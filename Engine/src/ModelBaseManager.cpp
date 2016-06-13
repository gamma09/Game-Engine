#include <GameAssert.h>

#include "MemorySetup.h"
#include "ModelBaseManager.h"
#include "ModelBase.h"

ModelBaseManager* ModelBaseManager::instance = nullptr;

#define MAX_MODEL_BASES_CREATED 128

ModelBaseManager* ModelBaseManager::Instance()
{
	GameAssert( ModelBaseManager::instance );
	return instance;
}

void ModelBaseManager::Create( Heap* managerHeap, uint32_t initialReserve, uint32_t refillSize )
{
	GameAssert( instance == 0 );
	instance = new( managerHeap, ALIGN_4 ) ModelBaseManager( initialReserve, refillSize );
}

void ModelBaseManager::Destroy()
{
	GameAssert( instance );
	instance->PreDestroy();
	delete instance;
	ModelBaseManager::instance = nullptr;
}

ModelBaseManager::ModelBaseManager( uint32_t initialReserve, uint32_t refillSize )
	: Manager( refillSize )
{
	Mem::createVariableBlockHeap( this->heap, MAX_MODEL_BASES_CREATED * sizeof( ModelBase ), "ModelBases" );
	this->Init( initialReserve );
}

ModelBaseManager::~ModelBaseManager()
{
	Mem::destroyHeap( heap );
}

ModelBase* ModelBaseManager::Add( ID3D11Device* device, const char* archiveFilename )
{
	ModelBase* model = static_cast<ModelBase*>( this->Add_Object() );
	model->Set( device, archiveFilename );
	return model;
}

ManagedObject* ModelBaseManager::Make_Object() const
{
	return new( this->heap, ALIGN_16 ) ModelBase();
}

void ModelBaseManager::Delete_Object( ManagedObject* obj ) const
{
	ModelBase* model = static_cast<ModelBase*>( obj );
	delete model;
}