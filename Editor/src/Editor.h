#pragma once

#include <Engine.h>
#include <BrowserListener.h>

#include <Timer.h>
#include <Time.h>
#include <Camera.h>
#include <Browser.h>

#include "Actor.h"

using namespace CLI;

class Heap;
class DirectionLight;

class Editor : public Engine, public BrowserListener
{
public:
	// constructor
	Editor( const char* windowName, const int Width, const int Height );

	//-----------------------------------------------------
	// Engine implementation
	//-----------------------------------------------------
	virtual void LoadContent() override;
	virtual void Update() override;
	virtual void Draw() override;
	virtual void UnLoadContent() override;

	// We only want to close the engine window if the browser says we can
	virtual void CloseEngine() override;

	//-----------------------------------------------------
	// BrowserListener implementation
	//-----------------------------------------------------
	virtual void OnModelSelected( ContentObject* model ) override;
	virtual void OnModelDeselected() override;

	virtual void OnActorSelected( ContentObject* actor ) override;
	virtual void OnActorDeselected() override;

	virtual void OnActorCreated( ContentObject* model ) override;
	virtual void OnActorDeleted( ContentObject* actor ) override;

	virtual void OnExit() override;

	virtual void OnNewLevel() override;
	virtual void OnOpenLevel( const char* filepath ) override;
	virtual void OnSaveLevel( const char* filepath ) override;
	virtual void OnLoadModel( const char* filepath ) override;


private:
	// prevent them from calling wrong call
	Editor() = delete;

	CLI::Browser* browser;

	//bool inModal;

	Timer updateTimer;
	Time totalTime;

	Camera* moveableCamera;
	Actor* actor[4];
	DirectionLight* light;
	bool wasCulled;
};