#pragma once

#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <Windows.h>
#include <d3d11_1.h>
#include <thread>
#include <stdio.h>
#include <string.h>

#include "LitTextureMaterial.h"
#include "UnlitTextureMaterial.h"
#include "WireframeMaterial.h"
#include "AppInfo.h"

class bad_function_call;
class Heap;

class Engine abstract
{
public:
	Engine( const char* windowName, const int Width,const int Height);
	virtual ~Engine();

	// required overloading
	virtual void LoadContent() abstract;
	virtual void Update() abstract;
	virtual void Draw() abstract;
	virtual void UnLoadContent() abstract;

	void run();

private:
	void CreateEngineWindow();
	void SetupDirect3D();

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

public:
	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;

	LitTextureMaterial* litTextureMaterial;
	UnlitTextureMaterial* unlitTextureMaterial;
	WireframeMaterial* wireframeMaterial;

private:
	IDXGISwapChain* swapChain;
	ID3D11RenderTargetView* renderTarget;
	ID3D11Texture2D* depthStencil;
	ID3D11DepthStencilView* depthStencilView;
	// ID3D11RasterizerState* rasterizer; front is CW
	unsigned int vsyncInterval;

	HWND hWindow;
	bool isOpen;
	std::thread updateThread;

public:
	Heap* managerHeap;

	AppInfo info;
};

