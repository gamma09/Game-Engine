#pragma once

#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <Windows.h>
#ifdef _DEBUG
#include <dxgi1_3.h>
#include <dxgidebug.h>
#endif
#include <d3d11_2.h>
#include <thread>
#include <stdio.h>
#include <string.h>
#include <atomic>
#include <Timer.h>
#include <Time.h>

#include "LitTextureMaterial.h"
#include "UnlitTextureMaterial.h"
#include "WireframeMaterial.h"
#include "AppInfo.h"
#include "EngineMessages.h"
#include "YieldMutex.h"
#include "SceneAsset.h"
#include "UpdateEventManager.h"
#include "CalculateSkeletonShader.h"

class Heap;
class DirectionLight;

class Engine abstract
{
public:
	Engine( const char* windowName, const int Width,const int Height);
	virtual ~Engine();

	// required overloading
	virtual void LoadContent() abstract;
	virtual void Update( uint32_t totalTimeMillis );
	virtual void Draw();
	virtual void UnLoadContent() abstract;

	virtual void CloseEngine();

	void run();
	void Close();

private:
	void CreateEngineWindow();
	void SetupDirect3D();
	D3D_FEATURE_LEVEL CreateDirectXDevice( IDXGIFactory1* dxgiFactory1 );
	void CreateSwapChain( IDXGIFactory1* dxgiFactory1 );
	void SetupDirectXDebugging();
	void SetupRenderView();
	void CreateViewport();
	void SetupCursor();

	static void UpdateThreadEntry( Engine* engine );
	static LRESULT CALLBACK WindowCallback( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );


	// private: --------------------------------------
private:
	// force to use the appropriate constructor
	Engine() = delete;
	Engine( const Engine& ) = delete;
	Engine& operator=( const Engine& ) = delete;

	// private functions
	void PreLoadContent();

protected:
	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;
	ID3DUserDefinedAnnotation* annotation;

	Timer updateTimer;
	Time totalTime;
	UpdateEventManager updateEventManager;
	SceneAsset* sceneAsset;

	// TODO move lights to scene
	DirectionLight* light;

	// TODO move materials to scene
	CalculateSkeletonShader* skeletonShader;
	LitTextureMaterial* litTextureMaterial;
	UnlitTextureMaterial* unlitTextureMaterial;
	WireframeMaterial* wireframeMaterial;

	YieldMutex drawMutex;
	YieldMutex updateMutex;

	IDXGISwapChain* swapChain;
	ID3D11RenderTargetView* renderTarget;
	ID3D11Texture2D* depthStencil;
	ID3D11DepthStencilView* depthStencilView;
	unsigned int vsyncInterval;

	HWND hWindow;
	AppInfo info;

#ifdef _DEBUG
	ID3D11Debug* debugInterface;
#endif

private:
	bool isOpen;
	
	std::thread updateThread;

	Heap* managerHeap;



	friend class ChangeSceneUpdateEvent;
};

