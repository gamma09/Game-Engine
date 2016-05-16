#include <initguid.h>
#include <DirectXColors.h>
#include <string.h>
#include <DebuggerSetup.h>
#include <GameAssert.h>
#include <mem.h>

#include "Camera.h"
#include "Engine.h"
#include "GlobalHeaps.h"
#include "ActorManager.h"
#include "CameraManager.h"
#include "DirectionLightManager.h"
#include "KeyBindingManager.h"
#include "ModelBaseManager.h"
#include "TextureManager.h"
#include "DirectXAssert.h"



void Engine::run()
{
	this->PreLoadContent();

	LoadContent();

	this->updateThread = std::thread( UpdateThreadEntry, this );

	MSG msg ={ 0 };
	while( this->isOpen )
	{
		if( PeekMessage( &msg, this->hWindow, 0, 0, PM_REMOVE ) )
		{

			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		else
		{
			this->annotation->BeginEvent( L"Engine Frame" );
			{
				this->deviceContext->ClearRenderTargetView( this->renderTarget, DirectX::Colors::LightBlue );
				this->deviceContext->ClearDepthStencilView( this->depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0 );

				Draw();

				this->swapChain->Present( this->vsyncInterval, 0 );
			}
			this->annotation->EndEvent();
		}
	}

	this->updateThread.join();

	UnLoadContent();
}

const static Time MILLISECOND = Time( TIME_ONE_MILLISECOND );

void Engine::Update( uint32_t totalTimeMillis )
{
	KeyBindingManager::Instance()->Check_Input();
	this->sceneAsset->Update( totalTimeMillis );
}

void Engine::Draw()
{
	this->annotation->BeginEvent( L"Engine::Draw()" );
	{
		Camera* activeCamera = CameraManager::Instance()->Get_Active_Camera();
		activeCamera->Update_Buffers( this->deviceContext );

		this->skeletonShader->CalculateSkeletons( this->deviceContext, this->annotation );

		this->annotation->BeginEvent( L"Draw Lit Material" );
		{
			this->litTextureMaterial->Draw( activeCamera, this->light, this->deviceContext, this->annotation );
		}
		this->annotation->EndEvent();

		this->annotation->BeginEvent( L"Draw Unlit Material" );
		{
			this->unlitTextureMaterial->Draw( activeCamera, this->light, this->deviceContext, this->annotation );
		}
		this->annotation->EndEvent();

		this->annotation->BeginEvent( L"Draw Wireframe Material" );
		{
			this->wireframeMaterial->Draw( activeCamera, this->light, this->deviceContext, this->annotation );
		}
		this->annotation->EndEvent();
	}
	this->annotation->EndEvent();
}

void Engine::Close()
{
	PostMessage( this->hWindow, WM_CLOSE, 0, 0 );
}

void Engine::UpdateThreadEntry( Engine* engine )
{
	while( engine->isOpen )
	{
			
		Time timeSinceUpdate = engine->updateTimer.toc();
		engine->updateTimer.tic();
		engine->totalTime += timeSinceUpdate;
		uint32_t totalTimeMillis = Time::quotient( engine->totalTime, MILLISECOND );
		
		engine->updateEventManager.ProcessPreUpdateEvents( totalTimeMillis );
		{
			YieldMutex::Lock updateLock( engine->updateMutex );
			engine->Update( totalTimeMillis );
		}
		engine->updateEventManager.ProcessPostUpdateEvents( totalTimeMillis );
	}
}

LRESULT CALLBACK Engine::WindowCallback( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	Engine* engine = (Engine*) GetWindowLongPtr( hWnd, GWLP_USERDATA );

	switch( message )
	{
		case WM_CLOSE:
			engine->CloseEngine();
			break;

		case WM_DESTROY:
			PostQuitMessage( 0 );
			engine->isOpen = false;
			break;

		case WM_PAINT:
			PAINTSTRUCT ps;
			BeginPaint( engine->hWindow, &ps );
			EndPaint( engine->hWindow, &ps );
			break;

		case WM_ENGINE_DESTROY_WINDOW:
			DestroyWindow( engine->hWindow );
			break;

		default:
			return DefWindowProc( hWnd, message, wParam, lParam );
	}

	return 0;
}

void Engine::CloseEngine()
{
	DestroyWindow( this->hWindow );
}

void Engine::PreLoadContent()
{
	this->CreateEngineWindow();
	this->SetupDirect3D();

	this->skeletonShader = new CalculateSkeletonShader( this->device );
	this->litTextureMaterial = new LitTextureMaterial( this->device );
	this->unlitTextureMaterial = new UnlitTextureMaterial( this->device );
	this->wireframeMaterial = new WireframeMaterial( this->device );

	TextureManager::Instance()->Create_Default_Texture( this->device );
}






#pragma region Windows and DirectX Environment Setup

#define WINDOW_CLASS_NAME "GameEngineWindow"

void Engine::CreateEngineWindow()
{
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WindowCallback;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetModuleHandle( NULL );
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor( NULL, IDC_ARROW );
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = WINDOW_CLASS_NAME;
	RegisterClass( &wc );

	DWORD exStyle = WS_EX_APPWINDOW;
	DWORD style = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE;


	if( this->info[Fullscreen] )
	{
		// TODO what happens if this is removed?
		style |= WS_POPUP;
	}
	else
	{
		style |= WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
	}

	RECT rect;

	// Create a window rectangle
	rect.left = (long) 0;
	rect.right = ( long ) this->info[Window_Width] - 1;
	rect.top = (long) 0;
	rect.bottom = ( long ) this->info[Window_Height] - 1;

	// Adjust according to window styles
	AdjustWindowRectEx( &rect, style, FALSE, exStyle );

	// Calculate width and height of full window
	int fullWidth = rect.right - rect.left + 1;
	int fullHeight = rect.bottom - rect.top + 1;

	// Adjust window position to working area (e.g. if the task bar is at
	// the top of the display). Fullscreen windows are always opened in
	// the upper left corner regardless of the desktop working area.
	RECT wa;
	if( this->info[Fullscreen] )
	{
		wa.left = wa.top = 0;
	}
	else
	{
		SystemParametersInfo( SPI_GETWORKAREA, 0, &wa, 0 );
	}

	this->hWindow = CreateWindowExA( exStyle, WINDOW_CLASS_NAME, this->info[Window_Title], style, wa.left, wa.top, fullWidth, fullHeight, NULL, NULL, NULL, NULL );
	if( this->hWindow == nullptr )
	{
		DWORD lastError = GetLastError();
		lastError;
		out( "Could not create window - error code = %d", lastError );
	}
	GameCheckFatal( this->hWindow, "Could not create window." );

	SetWindowLongPtr( this->hWindow, GWLP_USERDATA, ( LONG_PTR ) this );
}

// Priority for feature level - we want DirectX 11.1 if possible, but otherwise just try to load as high a version as possible
static const D3D_FEATURE_LEVEL REQUESTED_FEATURE_LEVEL_PRIORITY[] =
{
	D3D_FEATURE_LEVEL_11_1,
	D3D_FEATURE_LEVEL_11_0
};

static const unsigned int REQUESTED_FEATURE_LEVEL_PRIORITY_COUNT = sizeof( REQUESTED_FEATURE_LEVEL_PRIORITY ) / sizeof( D3D_FEATURE_LEVEL );


static IDXGIFactory1* CreateFactoryForDXGI()
{
	IDXGIFactory1* dxgiFactory1 = nullptr;
	GameCheckFatalDx( CreateDXGIFactory1( __uuidof( IDXGIFactory1 ), reinterpret_cast<void**>( &dxgiFactory1 ) ), "Could not create the DXGI factory." );

	return dxgiFactory1;
}

// We need to enumerate what adapters (and displays) exist on the system and find the first one with a display plugged in and is hardware based, but we will settle for
// one with a software driver if a hardware one cannot be found. If we only find adapters with no displays plugged in, bad shit happened.
static IDXGIAdapter1* FindBestPossibleAdapter( IDXGIFactory1* dxgiFactory1 )
{
	IDXGIAdapter1* adapterToUse = nullptr;
	bool adapterToUseIsHardware = false;

	IDXGIAdapter1* adapter = nullptr;
	for( int i = 0; dxgiFactory1->EnumAdapters1( i, &adapter ) != DXGI_ERROR_NOT_FOUND; i++ )
	{
		IDXGIOutput* output = nullptr;
		bool hasOutputs = ( adapter->EnumOutputs( 0, &output ) != DXGI_ERROR_NOT_FOUND );
		if( hasOutputs )
		{
			output->Release();
			DXGI_ADAPTER_DESC1 desc;
			GameCheckFatalDx( adapter->GetDesc1( &desc ), "Could not retrieve info about the adapter." );
			if( desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE && !adapterToUse )
			{
				adapterToUse = adapter;
				adapterToUse->AddRef();
				adapterToUseIsHardware = false;
			}
			else if( !adapterToUse )
			{
				adapterToUse = adapter;
				adapterToUse->AddRef();
				adapterToUseIsHardware = true;
			}
		}

		adapter->Release();
	}

	GameCheckFatal( adapterToUse, "Could not find any adapters with a monitor attached!" );
	return adapterToUse;
}

static D3D_FEATURE_LEVEL CreateDirectXDevice( IDXGIFactory1* dxgiFactory1, ID3D11Device** ppDevice, ID3D11DeviceContext** ppContext )
{
#ifdef _DEBUG
	static const unsigned int CreateDeviceFlags = D3D11_CREATE_DEVICE_DEBUG;
#else
	static const unsigned int CreateDeviceFlags = 0;
#endif
	
	D3D_FEATURE_LEVEL deviceFeatureLevel;

	// Find an appropriate display adapter
	// 1. It must have a monitor plugged in
	// 2. Favor hardware over software
	IDXGIAdapter1* adapter = FindBestPossibleAdapter( dxgiFactory1 );

	// first try to load driver with full feature set (DirectX 11.1/11.2)
	HRESULT hr = D3D11CreateDevice( adapter,                                // Use the adapter we selected (our priority was Hardware driver > Software driver)
									D3D_DRIVER_TYPE_UNKNOWN,                // We chose an adapter ourselves, so the driver has already been determined
									NULL,                                   // We chose an adapter ourselves, so no DLL module required
									CreateDeviceFlags,                      // Flags for creating the D3D device
									REQUESTED_FEATURE_LEVEL_PRIORITY,       // What features we are requesting in order by priority (11.1 plz!)
									REQUESTED_FEATURE_LEVEL_PRIORITY_COUNT, // Length of the featureLevels array
									D3D11_SDK_VERSION,                      // We want Direct3D sdk version from DirectX 11
									ppDevice,                               // We want a pointer to the Direct3D device that was created so that we can set up a swap chain
									&deviceFeatureLevel,                    // We want to know what DirectX feature level we're using
									ppContext );                            // We want a pointer to the Direct3D device context

	if( hr == E_INVALIDARG )
	{
		// We must be on a DirectX 11.0 platform...
		hr = D3D11CreateDevice( adapter,                                    // use the default adapter - change this to change which screen to use maybe?
								D3D_DRIVER_TYPE_UNKNOWN,                    // We chose an adapter ourselves, so the driver has already been determined
								NULL,                                       // We chose an adapter ourselves, so no DLL module required
								CreateDeviceFlags,                          // Flags for creating the D3D device
								&REQUESTED_FEATURE_LEVEL_PRIORITY[1],       // What features we are requesting in order by priority (11.0 plz!)
								REQUESTED_FEATURE_LEVEL_PRIORITY_COUNT - 1, // Length of the featureLevels array
								D3D11_SDK_VERSION,                          // We want Direct3D sdk version from DirectX 11
								ppDevice,                                   // We want a pointer to the Direct3D device that was created so that we can set up a swap chain
								&deviceFeatureLevel,                        // We want to know what DirectX feature level we're using
								ppContext );                                // We want a pointer to the Direct3D device context
	}

	GameCheckFatal( SUCCEEDED( hr ), "Failed to create D3D11 device." );

	adapter->Release();

	return deviceFeatureLevel;
}

void Engine::CreateSwapChain( IDXGIFactory1* dxgiFactory1 )
{
	UINT maxQualityLevel;
	this->device->CheckMultisampleQualityLevels( DXGI_FORMAT_R8G8B8A8_UNORM, this->info[Samples], &maxQualityLevel );
	while( !maxQualityLevel )
	{
		this->info.DecreaseSampleCount();
		this->device->CheckMultisampleQualityLevels( DXGI_FORMAT_R8G8B8A8_UNORM, this->info[Samples], &maxQualityLevel );
	}

	IDXGIFactory2* dxgiFactory2 = nullptr;
	dxgiFactory1->QueryInterface( __uuidof( IDXGIFactory2 ), reinterpret_cast<void**>( &dxgiFactory2 ) );
	if( dxgiFactory2 != nullptr )
	{
		// We are using DirectX 11.1 or later
		DXGI_SWAP_CHAIN_DESC1 sd;
		sd.Width = this->info[Window_Width];
		sd.Height = this->info[Window_Height];
		sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.Stereo = FALSE;
		sd.SampleDesc.Count = this->info[Samples];
		if( this->info[Samples] == 1 )
		{
			sd.SampleDesc.Quality = 0;
		}
		else
		{
			sd.SampleDesc.Quality = maxQualityLevel - 1;
		}

		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.BufferCount = 1; // Only 1 back buffer (MSDN is a bit misleading on this - there is always implicitly one front buffer)
		sd.Scaling = DXGI_SCALING_ASPECT_RATIO_STRETCH;
		sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		sd.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
		sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsd;
		DXGI_SWAP_CHAIN_FULLSCREEN_DESC* pFsd;

		if( this->info[Fullscreen] )
		{
			fsd.RefreshRate ={ this->info[Refresh_Rate], 1 };
			fsd.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
			fsd.Scaling = DXGI_MODE_SCALING_STRETCHED;
			fsd.Windowed = this->info[Fullscreen_Windowed] ? TRUE : FALSE;

			pFsd = &fsd;
		}
		else
		{
			pFsd = nullptr;
		}

		IDXGISwapChain1* tempSwapChain;
		GameCheckFatalDx( dxgiFactory2->CreateSwapChainForHwnd( this->device, this->hWindow, &sd, pFsd, nullptr, &tempSwapChain ), "Could not create swap chain." );
		GameCheckFatalDx( tempSwapChain->QueryInterface( __uuidof( IDXGISwapChain ), reinterpret_cast<void**>( &this->swapChain ) ), "Could not retrieve regular swap chain from SwapChain1" );
		tempSwapChain->Release();
		dxgiFactory2->Release();
	}
	else
	{
		// We are using DirectX 11.0
		DXGI_SWAP_CHAIN_DESC sd;
		sd.BufferDesc.Width = this->info[Window_Width];
		sd.BufferDesc.Height = this->info[Window_Height];
		sd.BufferDesc.RefreshRate ={ this->info[Refresh_Rate], 1 };
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		sd.BufferDesc.Scaling = DXGI_MODE_SCALING_STRETCHED;
		sd.SampleDesc.Count = this->info[Samples];
		if( this->info[Samples] == 1 )
		{
			sd.SampleDesc.Quality = 0;
		}
		else
		{
			sd.SampleDesc.Quality = maxQualityLevel - 1;
		}

		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.BufferCount = 1; // Front buffer is implicit - use 1 back buffer
		sd.OutputWindow = this->hWindow;
		sd.Windowed = TRUE; // Always set this to true, even when going fullscreen mode - if we are going fullscreen mode, call SetFullscreenState
		sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		GameCheckFatalDx( dxgiFactory1->CreateSwapChain( this->device, &sd, &this->swapChain ), "Could not create swap chain." );
		if( this->info[Fullscreen] )
		{
			this->swapChain->SetFullscreenState( TRUE, nullptr );
		}
	}

	// Alt-Enter shall not switch between windowed and fullscreen!
	dxgiFactory1->MakeWindowAssociation( this->hWindow, DXGI_MWA_NO_ALT_ENTER );
}

void Engine::SetupDirectXDebugging()
{
	GameCheckFatalDx( this->deviceContext->QueryInterface( __uuidof( ID3DUserDefinedAnnotation ), reinterpret_cast<void**>( &this->annotation ) ), "Couldn't retrieve a user annotation struct from Device Context." );

#ifdef _DEBUG
	GameCheckFatalDx( this->device->QueryInterface( __uuidof( ID3D11Debug ), reinterpret_cast<void**>( &this->debugInterface ) ), "Couldn't retrieve the debug layer from the Device." );
	GameCheckFatalDx( DXGIGetDebugInterface1( 0, __uuidof( IDXGIDebug ), reinterpret_cast<void**>( &this->lowLevelDebugInterface ) ), "Couldn't retrieve the low-level debug layer." );
#endif
}

void Engine::SetupRenderView()
{
	// Create render target view
	ID3D11Texture2D* pBackBuffer;
	GameCheckFatalDx( this->swapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), reinterpret_cast<void**>( &pBackBuffer ) ), "Could not retrieve back buffer." );
	GameCheckFatalDx( this->device->CreateRenderTargetView( pBackBuffer, nullptr, &this->renderTarget ), "Could not create render target." );
	pBackBuffer->Release();

	// Create depth stencil texture
	D3D11_TEXTURE2D_DESC depthDesc;
	depthDesc.Width = this->info[Window_Width];
	depthDesc.Height = this->info[Window_Height];
	depthDesc.MipLevels = 1;
	depthDesc.ArraySize = 1;
	depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthDesc.SampleDesc.Count = 1;
	depthDesc.SampleDesc.Quality = 0;
	depthDesc.Usage = D3D11_USAGE_DEFAULT;
	depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthDesc.CPUAccessFlags = 0;
	depthDesc.MiscFlags = 0;
	GameCheckFatalDx( this->device->CreateTexture2D( &depthDesc, nullptr, &this->depthStencil ), "Could not create depth stencil texture." );

	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Format = depthDesc.Format;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;
	dsvDesc.Flags = 0;
	GameCheckFatalDx( this->device->CreateDepthStencilView( this->depthStencil, &dsvDesc, &this->depthStencilView ), "Could not create depth stencil view." );
	
	// Setup the Output Merger stage
	this->deviceContext->OMSetRenderTargets( 1, &this->renderTarget, this->depthStencilView );
}

void Engine::CreateViewport()
{
	// Setup the viewport
	D3D11_VIEWPORT vp;
	vp.Width = ( float ) this->info[Window_Width];
	vp.Height = ( float ) this->info[Window_Height];
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;

	// Setup the Rasterizer stage
	this->deviceContext->RSSetViewports( 1, &vp );
}

void Engine::SetupCursor()
{
	if( this->info[Cursor] )
	{
		ReleaseCapture();
		ClipCursor( NULL );
		ShowCursor( TRUE );
	}
	else
	{
		RECT ClipWindowRect;

		ShowCursor( FALSE );

		// Clip cursor to the window
		if( GetWindowRect( this->hWindow, &ClipWindowRect ) )
		{
			ClipCursor( &ClipWindowRect );
		}

		// Capture cursor to user window
		SetCapture( this->hWindow );
	}
}

void Engine::SetupDirect3D()
{
	IDXGIFactory1* dxgiFactory1 = CreateFactoryForDXGI();

	CreateDirectXDevice( dxgiFactory1, &this->device, &this->deviceContext );
	CreateSwapChain( dxgiFactory1 );
	SetupDirectXDebugging();
	SetupRenderView();
	CreateViewport();
	SetupCursor();

	this->vsyncInterval = this->info[Vsync] ? 1 : 0;
	dxgiFactory1->Release();
}

#pragma endregion

Engine::Engine( const char* windowName, const int Width, const int Height )
	: info( windowName, 1 ),
	isOpen( true ),
	skeletonShader( nullptr ),
	litTextureMaterial( nullptr ),
	wireframeMaterial( nullptr ),
	unlitTextureMaterial( nullptr ),
	device( nullptr ),
	deviceContext( nullptr ),
	swapChain( nullptr ),
	renderTarget( nullptr ),
	depthStencil( nullptr ),
	depthStencilView( nullptr ),
	drawMutex(),
	updateMutex(),
	updateEventManager()
{
	this->info[Window_Width] = Width;
	this->info[Window_Height] = Height;
	this->info[Refresh_Rate] = 60;

	this->info[Cursor] = true;
	this->info[Fullscreen] = false;
	this->info[Vsync] = false;

	TemporaryHeap::Create();
	ConstantBufferHeap::Create();
	AnimHeap::Create();
	AssetHeap::Create();
	EventHeap::Create();
	ModelHeap::Create();

	Mem::createVariableBlockHeap( this->managerHeap, 4096 );

	ModelBaseManager::Create( this->managerHeap, 7, 1 );
	CameraManager::Create( this->managerHeap, 1, 1 );
	TextureManager::Create( this->managerHeap, 10, 3 );
	ActorManager::Create( this->managerHeap, 12, 1 );
	DirectionLightManager::Create( this->managerHeap, 1, 1 );
	KeyBindingManager::Create( this->managerHeap, 4, 2 );
}

Engine::~Engine()
{
	delete skeletonShader;
	delete litTextureMaterial;
	delete unlitTextureMaterial;
	delete wireframeMaterial;

	KeyBindingManager::Destroy();
	DirectionLightManager::Destroy();
	ActorManager::Destroy();
	TextureManager::Destroy();
	CameraManager::Destroy();
	ModelBaseManager::Destroy();

	Mem::destroyHeap( this->managerHeap );

	ModelHeap::Destroy();
	EventHeap::Destroy();
	AssetHeap::Destroy();
	AnimHeap::Destroy();
	TemporaryHeap::Destroy();
	ConstantBufferHeap::Destroy();

	this->annotation->Release();
	this->depthStencil->Release();
	this->depthStencilView->Release();
	this->renderTarget->Release();
	this->swapChain->Release();
	this->deviceContext->Release();

#ifdef _DEBUG
	this->debugInterface->ReportLiveDeviceObjects( D3D11_RLDO_SUMMARY | D3D11_RLDO_DETAIL );
	this->debugInterface->Release();

	this->lowLevelDebugInterface->ReportLiveObjects( DXGI_DEBUG_DXGI, DXGI_DEBUG_RLO_ALL );
	this->lowLevelDebugInterface->Release();
#endif
	
	this->device->Release();
}
