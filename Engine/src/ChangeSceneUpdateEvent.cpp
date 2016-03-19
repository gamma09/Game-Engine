#include "ChangeSceneUpdateEvent.h"
#include "Engine.h"
#include "SceneAsset.h"
#include "MemorySetup.h"

ChangeSceneUpdateEvent::ChangeSceneUpdateEvent( Engine& inEngine )
	: UpdateEvent(),
	engine( inEngine )
{
	// Do nothing
}

ChangeSceneUpdateEvent::~ChangeSceneUpdateEvent()
{
	// Do nothing
}

bool ChangeSceneUpdateEvent::operator()( uint32_t )
{
	YieldMutex::Lock lock( engine.drawMutex );
	engine.sceneAsset->DeleteAsset();
	engine.sceneAsset = this->CreateScene();

	return true;
}
