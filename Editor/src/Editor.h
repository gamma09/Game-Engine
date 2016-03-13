#pragma once

#include <Engine.h>

#include <Timer.h>
#include <Time.h>
#include <Camera.h>

#include "Actor.h"


class Heap;
class DirectionLight;

class Editor : public Engine
{
public:
	// constructor
	Editor( const char* windowName, const int Width, const int Height );

	virtual void LoadContent() override;
	virtual void Update() override;
	virtual void Draw() override;
	virtual void UnLoadContent() override;

private:
	// prevent them from calling wrong call
	Editor();

	Timer updateTimer;
	Time totalTime;

	Camera* moveableCamera;
	Actor* actor[4];
	DirectionLight* light;
	bool wasCulled;
};