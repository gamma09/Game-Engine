#pragma once

#include <stdint.h>

#include <Manager.h>

class DirectionLight;
class Heap;

class DirectionLightManager : public Manager
{
public:
	static DirectionLightManager* Instance();
		
	static void Create(Heap* managerHeap, uint32_t initialReserve, uint32_t refillSize);
	static void Destroy();

	DirectionLight* Add(float x, float y, float z, float r, float g, float b, float a);
		
		

protected:
	virtual ManagedObject* Make_Object() const override;
	virtual void Delete_Object(ManagedObject* obj) const override;

private:
	// creation/destruction of manager can only be done by the manager
	DirectionLightManager(uint32_t initialReserve, uint32_t refillSize);
	virtual ~DirectionLightManager();

	// no copying allowed! and no locals either!
	DirectionLightManager();
	DirectionLightManager(const DirectionLightManager& manager);
	DirectionLightManager& operator=(const DirectionLightManager& manager);

private:
	Heap* heap;

	static DirectionLightManager* instance;
};