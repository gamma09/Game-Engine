#pragma once

#include <Engine.h>

#include <Timer.h>
#include <Time.h>

#include "Actor.h"

class Camera;
class Heap;
class DirectionLight;

class Editor : public Engine
{
public:
	// constructor
	Editor( const char* windowName, const int Width, const int Height );

	virtual void Initialize();
	virtual void LoadContent();
	virtual void Update();
	virtual void Draw();
	virtual void UnLoadContent();

	virtual void ClearBufferFunc();

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