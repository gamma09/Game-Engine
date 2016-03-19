#include <SceneAsset.h>
#include <MemorySetup.h>
#include "NewSceneUpdateEvent.h"

NewSceneUpdateEvent::NewSceneUpdateEvent( Engine& engine )
	: ChangeSceneUpdateEvent( engine )
{
	// Do nothing
}

NewSceneUpdateEvent::~NewSceneUpdateEvent()
{
	// Do nothing
}

SceneAsset* NewSceneUpdateEvent::CreateScene()
{
	return new( AssetHeap::Instance(), ALIGN_4 ) SceneAsset( "Untitled Scene" );
}
