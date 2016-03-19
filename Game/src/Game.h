#pragma once

#include <Engine.h>

#include <Timer.h>
#include <Time.h>
#include <Camera.h>

#include "Actor.h"


class Heap;
class DirectionLight;

class Game : public Engine
{
public:
	// constructor
	Game( const char* windowName, const int Width, const int Height );

	virtual void LoadContent() override;
	virtual void Update( uint32_t updateTime ) override;
	virtual void UnLoadContent() override;

private:
	// prevent them from calling wrong call
	Game();

	Camera* moveableCamera;
};