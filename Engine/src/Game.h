#pragma once

#include "Engine.h"

#include <MathEngine.h>
#include <Timer.h>
#include <Time.h>

// ToDo get rid of the projection, move to the camera
#include "Actor.h"

class Camera;
class Heap;
class DirectionLight;

class Game: public Engine
{
public:
	// constructor
	Game(const char* windowName, const int Width,const int Height);

	virtual void Initialize();
	virtual void LoadContent();
	virtual void Update() ;
	virtual void Draw();
	virtual void UnLoadContent();

	virtual void ClearBufferFunc();

private:	
	// prevent them from calling wrong call
	Game();

	Timer updateTimer;
	Time totalTime;

	Camera* moveableCamera;
	Actor* actor[4];
	DirectionLight* light;
	bool wasCulled;

	Heap* managerHeap;
	Heap* materialHeap;
};