#include <GameAssert.h>

#include "MemorySetup.h"
#include "DirectionLightManager.h"
#include "DirectionLight.h"


DirectionLightManager* DirectionLightManager::instance;

DirectionLightManager* DirectionLightManager::Instance()
{
	GameAssert(instance != 0);

	return instance;
}
		
void DirectionLightManager::Create(Heap* managerHeap, uint32_t initialReserve, uint32_t refillSize)
{
	managerHeap;

	GameAssert(instance == 0);

	instance = new(managerHeap, ALIGN_4) DirectionLightManager(initialReserve, refillSize);
}

void DirectionLightManager::Destroy()
{
	GameAssert(instance != 0);
	delete instance;
	instance = 0;
}

DirectionLight* DirectionLightManager::Add(float x, float y, float z, float r, float g, float b, float a)
{
	DirectionLight* light = static_cast<DirectionLight*>(this->Add_Object());
	light->Set(x, y, z, r, g, b, a);
	return light;
}


ManagedObject* DirectionLightManager::Make_Object() const
{
	return new(this->heap, ALIGN_16) DirectionLight();
}

void DirectionLightManager::Delete_Object(ManagedObject* obj) const
{
	delete obj;
}


DirectionLightManager::DirectionLightManager(uint32_t initialReserve, uint32_t refillSize) :
	Manager(refillSize)
{
	// Hey, might as well allocate a whole page of memory...
	// DO NOT CHANGE TO FIX SIZE HEAP! DirectionLight needs to be 16-byte aligned...
	Mem::createVariableBlockHeap( this->heap, 4096 );

	this->Init(initialReserve);
}

DirectionLightManager::~DirectionLightManager()
{
	Destroy_Objects();
	Mem::destroyHeap( heap );
}