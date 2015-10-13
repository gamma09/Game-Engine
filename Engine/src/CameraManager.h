#pragma once

#include <stdint.h>

#include <Manager.h>

class Camera;
class Heap;
struct PerspectiveData;
struct ViewportData;
struct OrientationData;


class CameraManager : public Manager
{
public:
	static CameraManager* Instance();
		
	static void Create(Heap* managerHeap, uint32_t initialReserve, uint32_t refillSize);
	static void Destroy();

	Camera* const Add(const ViewportData& viewport, const PerspectiveData& perspective, const OrientationData& orientation);
	Camera* const Get_Active_Camera() const;
	void Set_Active_Camera(Camera* const camera);
		

protected:
	virtual ManagedObject* Make_Object() const override;
	virtual void Delete_Object(ManagedObject* obj) const override;

private:
	// creation/destruction of manager can only be done by the manager
	CameraManager(uint32_t initialReserve, uint32_t refillSize);
	virtual ~CameraManager();

	// no copying allowed! and no locals either!
	CameraManager();
	CameraManager(const CameraManager& manager);
	CameraManager& operator=(const CameraManager& manager);

private:
	Heap* heap;
	Camera* activeCamera;

	static CameraManager* instance;
};