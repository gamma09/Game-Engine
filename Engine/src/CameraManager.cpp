#include <assert.h>

#include "MemorySetup.h"
#include "CameraManager.h"
#include "Camera.h"

CameraManager* CameraManager::instance;

CameraManager* CameraManager::Instance()
{
	assert(instance != 0);

	return instance;
}
		
void CameraManager::Create(Heap* managerHeap, uint32_t initialReserve, uint32_t refillSize)
{
	managerHeap;

	assert(instance == 0);
	
	instance = new(managerHeap, ALIGN_4) CameraManager(initialReserve, refillSize);
}

void CameraManager::Destroy()
{
	assert(instance != 0);

	instance->Destroy_Objects();
	MemReturnCode status = Mem::destroyHeap(instance->heap);
	status;
	assert(status == Mem_OK);
	delete instance;
	instance = 0;
}

Camera* const CameraManager::Add(const ViewportData& viewport, const PerspectiveData& perspective, const OrientationData& orientation)
{
	Camera* camera = static_cast<Camera*>(this->Add_Object());
	camera->Set(viewport, perspective, orientation);

	if (this->activeCamera == 0)
		this->activeCamera = camera;

	return camera;
}

Camera* const CameraManager::Get_Active_Camera() const
{
	return this->activeCamera;
}

void CameraManager::Set_Active_Camera(Camera* const camera)
{
	this->activeCamera = camera;
}


ManagedObject* CameraManager::Make_Object() const
{
	return new(this->heap, ALIGN_16) Camera();
}

void CameraManager::Delete_Object(ManagedObject* obj) const
{
	delete obj;
}


CameraManager::CameraManager(uint32_t initialReserve, uint32_t refillSize) :
	Manager(refillSize),
	activeCamera(0)
{
	// Hey, might as well allocate a whole page of memory...
	MemReturnCode status = Mem::createHeap(this->heap, 4096, "Camera Heap");
	status;
	assert(status == Mem_OK);

	this->Init(initialReserve);
}

CameraManager::~CameraManager()
{
	// Do nothing
}