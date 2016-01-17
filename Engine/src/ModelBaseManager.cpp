#include <GameAssert.h>

#include "MemorySetup.h"
#include "ModelBaseManager.h"
#include "ModelBase.h"

ModelBaseManager* ModelBaseManager::instance;


#define MAX_MODEL_BASES_CREATED 128



ModelBaseManager* ModelBaseManager::Instance()
{
	GameAssert(ModelBaseManager::instance != 0);

	return instance;
}

void ModelBaseManager::Create(Heap* managerHeap, uint32_t initialReserve, uint32_t refillSize)
{
	managerHeap;

	GameAssert(instance == 0);

	instance = new(managerHeap, ALIGN_4) ModelBaseManager(initialReserve, refillSize);
}

void ModelBaseManager::Destroy()
{
	GameAssert(instance != 0);

	instance->Destroy_Objects();
	GameVerify( Mem_OK == Mem::destroyHeap(instance->heap) );
	delete instance;
	ModelBaseManager::instance = 0;
}

ModelBaseManager::ModelBaseManager(uint32_t initialReserve, uint32_t refillSize) :
	Manager(refillSize)
{
	GameVerify( Mem_OK == Mem::createFixBlockHeap(this->heap, MAX_MODEL_BASES_CREATED, sizeof(ModelBase), "Model Base Heap") );
	this->Init(initialReserve);
}

ModelBaseManager::~ModelBaseManager()
{
	// Do nothing
}

ModelBase* ModelBaseManager::Add(const char* archiveFilename)
{
	ModelBase* model = static_cast<ModelBase*>(this->Add_Object());
	model->Set(archiveFilename);
	return model;
}
	
ManagedObject* ModelBaseManager::Make_Object() const
{
	return new(this->heap, ALIGN_4) ModelBase();
}

void ModelBaseManager::Delete_Object(ManagedObject* obj) const
{
	ModelBase* model = static_cast<ModelBase*>(obj);
	delete model;
}