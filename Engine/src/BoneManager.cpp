#include <GameAssert.h>

#include "MemorySetup.h"
#include "BoneManager.h"
#include "Bone.h"

#define MAX_BONES_CREATED 512

BoneManager* BoneManager::instance;

BoneManager* BoneManager::Instance()
{
	GameAssert(instance != 0);

	return instance;
}
		
void BoneManager::Create(Heap* managerHeap, uint32_t initialReserve, uint32_t refillSize)
{
	managerHeap;

	GameAssert(instance == 0);

	instance = new(managerHeap, ALIGN_4) BoneManager(initialReserve, refillSize);
}

void BoneManager::Destroy()
{
	GameAssert(instance != 0);

	delete instance;
}

Bone* const BoneManager::Add(const uint32_t boneIndex)
{
	Bone* bone = static_cast<Bone*>(this->Add_Object());
	bone->Set(boneIndex);

	return bone;
}

		
		

ManagedObject* BoneManager::Make_Object() const
{
	return new Bone();
}

void BoneManager::Delete_Object(ManagedObject* obj) const
{
	delete obj;
}
	

BoneManager::BoneManager(uint32_t initialReserve, uint32_t refillSize) :
	Manager(refillSize)
{
	GameVerify( Mem_OK == Mem::createFixBlockHeap( this->heap, MAX_BONES_CREATED, sizeof( Bone ), "Boneyard/Heap" ) );
	this->Init(initialReserve);
}

BoneManager::~BoneManager()
{
	// Do nothing
}