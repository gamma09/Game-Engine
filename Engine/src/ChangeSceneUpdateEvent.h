#pragma once

#include "UpdateEvent.h"

class Engine;
class SceneAsset;

class ChangeSceneUpdateEvent : public UpdateEvent
{
public:
	ChangeSceneUpdateEvent( Engine& engine );
	virtual ~ChangeSceneUpdateEvent() override;

	virtual bool operator()( uint32_t updateTimeMillis ) override;

	virtual SceneAsset* CreateScene() abstract;

private:
	Engine& engine;
};