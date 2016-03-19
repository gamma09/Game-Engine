#pragma once

#include <ChangeSceneUpdateEvent.h>

class Engine;

class NewSceneUpdateEvent : public ChangeSceneUpdateEvent
{
public:
	NewSceneUpdateEvent( Engine& engine );
	virtual ~NewSceneUpdateEvent() override;

	virtual SceneAsset* CreateScene() override;
};