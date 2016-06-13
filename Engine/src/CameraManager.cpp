#include <GameAssert.h>

#include "MemorySetup.h"
#include "CameraManager.h"
#include "Camera.h"

CameraManager* CameraManager::instance = nullptr;

CameraManager* CameraManager::Instance()
{
	GameAssert( instance );
	return instance;
}

void CameraManager::Create( Heap* managerHeap, uint32_t initialReserve, uint32_t refillSize )
{
	GameAssert( instance == 0 );
	instance = new( managerHeap, ALIGN_4 ) CameraManager( initialReserve, refillSize );
}

void CameraManager::Destroy()
{
	GameAssert( instance );
	instance->PreDestroy();
	delete instance;
	instance = nullptr;
}

Camera* const CameraManager::Add( ID3D11Device* device, const PerspectiveData& perspective, const OrientationData& orientation )
{
	Camera* camera = static_cast<Camera*>( this->Add_Object() );
	camera->Set( device, perspective, orientation );

	if( this->activeCamera == 0 )
	{
		this->activeCamera = camera;
	}

	return camera;
}

Camera* const CameraManager::Get_Active_Camera() const
{
	return this->activeCamera;
}

void CameraManager::Set_Active_Camera( Camera* const camera )
{
	this->activeCamera = camera;
}


ManagedObject* CameraManager::Make_Object() const
{
	return new( this->heap, ALIGN_16 ) Camera();
}

void CameraManager::Delete_Object( ManagedObject* obj ) const
{
	delete obj;
}


CameraManager::CameraManager( uint32_t initialReserve, uint32_t refillSize )
	: Manager( refillSize ),
	activeCamera( 0 )
{
	// Hey, might as well allocate a whole page of memory...
	Mem::createVariableBlockHeap( this->heap, 4096, "Cameras" );
	this->Init( initialReserve );
}

CameraManager::~CameraManager()
{
	Mem::destroyHeap( heap );
}