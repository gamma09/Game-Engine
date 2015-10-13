#pragma once

#include <cstdint>

#include <Manager.h>

class Heap;
class Bone;

class BoneManager : public Manager
{
public:
	static BoneManager* Instance();
		
	static void Create(Heap* managerHeap, uint32_t initialReserve, uint32_t refillSize);
	static void Destroy();

	Bone* const Add(const uint32_t boneIndex);

		
		

protected:
	virtual ManagedObject* Make_Object() const override;
	virtual void Delete_Object(ManagedObject* obj) const override;
	

private:
	BoneManager(uint32_t initialReserve, uint32_t refillSize);
	virtual ~BoneManager();


	BoneManager();
	BoneManager(const BoneManager& manager);
	BoneManager& operator=(const BoneManager& manager);

private:
	Heap* heap;

	static BoneManager* instance;
};