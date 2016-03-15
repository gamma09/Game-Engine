
#include <DirectXColors.h>
#include <string.h>
#include <DebuggerSetup.h>
#include <GameAssert.h>
#include <mem.h>

#include "Engine.h"
#include "GlobalHeaps.h"
#include "ActorManager.h"
#include "BoneManager.h"
#include "CameraManager.h"
#include "DirectionLightManager.h"
#include "KeyBindingManager.h"
#include "ModelBaseManager.h"
#include "TextureManager.h"

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
			this->deviceContext->ClearRenderTargetView( this->renderTarget, DirectX::Colors::LightBlue );
			this->deviceContext->ClearDepthStencilView( this->depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0 );

			Draw();

			this->swapChain->Present( this->vsyncInterval, 0 );
		}
	}

	this->updateThread.join();

	UnLoadContent();
}

void Engine::UpdateThreadEntry( Engine* engine )
{
	while( engine->isOpen )
	{
		engine->Update();
	}
}

LRESULT CALLBACK Engine::WindowCallback( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	Engine* engine = (Engine*) GetWindowLongPtr( hWnd, GWLP_USERDATA );

	switch( message )
	{
		case WM_CLOSE:
			DestroyWindow( engine->hWindow );
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

		default:
			return DefWindowProc( hWnd, message, wParam, lParam );
	}

	return 0;
}

void Engine::PreLoadContent()
{
	this->CreateEngineWindow();
	this->SetupDirect3D();

	this->litTextureMaterial = new LitTextureMaterial( this->device );
	this->unlitTextureMaterial = new UnlitTextureMaterial( this->device );
	this->wireframeMaterial = new WireframeMaterial( this->device );

	TextureManager::Instance()->Create_Default_Texture( this->device );
}

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









// First try to use a hardware driver; if that doesn't work, use a WARP driver (high performance software-based driver)
// If those fail, fall back to using a slow-as-hell reference driver
static const D3D_DRIVER_TYPE REQUESTED_DRIVER_TYPE_PRIORITY[] =
{
	D3D_DRIVER_TYPE_HARDWARE,
	D3D_DRIVER_TYPE_WARP,
	D3D_DRIVER_TYPE_REFERENCE
};

static const unsigned int REQUESTED_DRIVER_TYPE_PRIORITY_COUNT = sizeof( REQUESTED_DRIVER_TYPE_PRIORITY ) / sizeof( D3D_DRIVER_TYPE );

// Priority for feature level - we want DirectX 11.1 if possible, but otherwise just try to load as high a version as possible
static const D3D_FEATURE_LEVEL REQUESTED_FEATURE_LEVEL_PRIORITY[] =
{
	D3D_FEATURE_LEVEL_11_1,
	D3D_FEATURE_LEVEL_11_0,
	D3D_FEATURE_LEVEL_10_1,
	D3D_FEATURE_LEVEL_10_0
};

static const unsigned int REQUESTED_FEATURE_LEVEL_PRIORITY_COUNT = sizeof( REQUESTED_FEATURE_LEVEL_PRIORITY ) / sizeof( D3D_FEATURE_LEVEL );



void Engine::SetupDirect3D()
{
	unsigned int CreateDeviceFlags = 0;

#ifdef _DEBUG
	CreateDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL featureLevel;

	HRESULT hr;
	for( unsigned int i = 0; i < REQUESTED_DRIVER_TYPE_PRIORITY_COUNT; i++ )
	{
		// first try to load driver with full feature set
		hr = D3D11CreateDevice( NULL,                                   // use the default adapter - change this to change which screen to use maybe?
								REQUESTED_DRIVER_TYPE_PRIORITY[i],      // use the fastest possible D3D driver
								NULL,                                   // we aren't using D3D_DRIVER_TYPE_SOFTWARE, so the DLL module is NULL
								CreateDeviceFlags,                      // Flags for creating the D3D device
								REQUESTED_FEATURE_LEVEL_PRIORITY,       // What features we are requesting in order by priority
								REQUESTED_FEATURE_LEVEL_PRIORITY_COUNT, // Length of the featureLevels array
								D3D11_SDK_VERSION,                      // We want Direct3D sdk version from DirectX 11
								&this->device,                                // We want a pointer to the Direct3D device that was created so that we can set up a swap chain
								&featureLevel,                          // We want to know what DirectX feature level we're using
								&this->deviceContext );                       // We want a pointer to the Direct3D device context

		if( hr == E_INVALIDARG )
		{
			// We must be on a DirectX 11.0 platform...
			hr = D3D11CreateDevice( NULL,                                       // use the default adapter - change this to change which screen to use maybe?
									REQUESTED_DRIVER_TYPE_PRIORITY[i],          // use the fastest possible D3D driver
									NULL,                                       // we aren't using D3D_DRIVER_TYPE_SOFTWARE, so the DLL module is NULL
									CreateDeviceFlags,                          // Flags for creating the D3D device
									&REQUESTED_FEATURE_LEVEL_PRIORITY[1],       // What features we are requesting in order by priority
									REQUESTED_FEATURE_LEVEL_PRIORITY_COUNT - 1, // Length of the featureLevels array
									D3D11_SDK_VERSION,                          // We want Direct3D sdk version from DirectX 11
									&this->device,                                    // We want a pointer to the Direct3D device that was created so that we can set up a swap chain
									&featureLevel,                              // We want to know what DirectX feature level we're using
									&this->deviceContext );                           // We want a pointer to the Direct3D device context
		}

		if( SUCCEEDED( hr ) )
			break;
	}

	GameCheckFatal( SUCCEEDED( hr ), "Failed to create D3D11 device." );
	UINT maxQualityLevel;
	this->device->CheckMultisampleQualityLevels( DXGI_FORMAT_R8G8B8A8_UNORM, this->info[Samples], &maxQualityLevel );
	while( !maxQualityLevel )
	{
		this->info.DecreaseSampleCount();
		this->device->CheckMultisampleQualityLevels( DXGI_FORMAT_R8G8B8A8_UNORM, this->info[Samples], &maxQualityLevel );
	}

	IDXGIDevice* dxgiDevice = nullptr;
	GameCheckFatal( SUCCEEDED( this->device->QueryInterface( __uuidof( IDXGIDevice ), reinterpret_cast<void**>( &dxgiDevice ) ) ), "Couldn't retrieve DXGIDevice." );
	IDXGIAdapter* adapter = nullptr;
	GameCheckFatal( SUCCEEDED( dxgiDevice->GetAdapter( &adapter ) ), "Couldn't get the DXGI device adapter." );
	IDXGIFactory1* dxgiFactory = nullptr;
	GameCheckFatal( SUCCEEDED( adapter->GetParent( __uuidof( IDXGIFactory1 ), reinterpret_cast<void**>( &dxgiFactory ) ) ), "Couldn't retrieve the DXGIFactory1 from the adapter." );
	adapter->Release();
	dxgiDevice->Release();

	IDXGIFactory2* dxgiFactory2 = nullptr;
	dxgiFactory->QueryInterface( __uuidof( IDXGIFactory2 ), reinterpret_cast<void**>( &dxgiFactory2 ) );
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
		GameCheckFatal( SUCCEEDED( dxgiFactory2->CreateSwapChainForHwnd( this->device, this->hWindow, &sd, pFsd, nullptr, &tempSwapChain ) ), "Could not create swap chain." );
		GameCheckFatal( SUCCEEDED( tempSwapChain->QueryInterface( __uuidof( IDXGISwapChain ), reinterpret_cast<void**>( &this->swapChain ) ) ), "Could not retrieve regular swap chain from SwapChain1" );
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

		GameCheckFatal( SUCCEEDED( dxgiFactory->CreateSwapChain( this->device, &sd, &this->swapChain ) ), "Could not create swap chain." );
		if( this->info[Fullscreen] )
		{
			this->swapChain->SetFullscreenState( TRUE, nullptr );
		}
	}

	// Alt-Enter shall not switch between windowed and fullscreen!
	dxgiFactory->MakeWindowAssociation( this->hWindow, DXGI_MWA_NO_ALT_ENTER );
	dxgiFactory->Release();

	// Create render target view
	ID3D11Texture2D* pBackBuffer;
	GameCheckFatal( SUCCEEDED( this->swapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), reinterpret_cast<void**>( &pBackBuffer ) ) ), "Could not retrieve back buffer." );
	GameCheckFatal( SUCCEEDED( this->device->CreateRenderTargetView( pBackBuffer, nullptr, &this->renderTarget ) ), "Could not create render target." );
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
	GameCheckFatal( SUCCEEDED( this->device->CreateTexture2D( &depthDesc, nullptr, &this->depthStencil ) ), "Could not create depth stencil texture." );

	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Format = depthDesc.Format;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;
	dsvDesc.Flags = 0;
	GameCheckFatal( SUCCEEDED( this->device->CreateDepthStencilView( this->depthStencil, &dsvDesc, &this->depthStencilView ) ), "Could not create depth stencil view." );

	this->deviceContext->OMSetRenderTargets( 1, &this->renderTarget, this->depthStencilView );

	// Setup the viewport
	D3D11_VIEWPORT vp;
	vp.Width = ( float ) this->info[Window_Width];
	vp.Height = ( float ) this->info[Window_Height];
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	this->deviceContext->RSSetViewports( 1, &vp );

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

	this->vsyncInterval = this->info[Vsync] ? 1 : 0;
}

Engine::Engine( const char* windowName, const int Width, const int Height )
	: info( windowName, 1 ),
	isOpen( true ),
	litTextureMaterial( nullptr ),
	wireframeMaterial( nullptr ),
	unlitTextureMaterial( nullptr ),
	device( nullptr ),
	deviceContext( nullptr ),
	swapChain( nullptr ),
	renderTarget( nullptr ),
	depthStencil( nullptr ),
	depthStencilView( nullptr )
{
	this->info[Window_Width] = Width;
	this->info[Window_Height] = Height;
	this->info[Refresh_Rate] = 60;

	this->info[Cursor] = true;
	this->info[Fullscreen] = false;
	this->info[Vsync] = true;

	TemporaryHeap::Create();
	ConstantBufferHeap::Create();
	AnimHeap::Create();

	Mem::createVariableBlockHeap( this->managerHeap, 4096 );

	ModelBaseManager::Create( this->managerHeap, 7, 1 );
	CameraManager::Create( this->managerHeap, 1, 1 );
	TextureManager::Create( this->managerHeap, 10, 3 );
	ActorManager::Create( this->managerHeap, 12, 1 );
	DirectionLightManager::Create( this->managerHeap, 1, 1 );
	KeyBindingManager::Create( this->managerHeap, 4, 2 );
	BoneManager::Create( this->managerHeap, 100, 10 );
}

Engine::~Engine()
{
	KeyBindingManager::Destroy();
	DirectionLightManager::Destroy();
	ActorManager::Destroy();
	TextureManager::Destroy();
	CameraManager::Destroy();
	ModelBaseManager::Destroy();
	BoneManager::Destroy();

	Mem::destroyHeap( this->managerHeap );

	AnimHeap::Destroy();
	TemporaryHeap::Destroy();
	ConstantBufferHeap::Destroy();

	this->depthStencil->Release();
	this->depthStencilView->Release();
	this->renderTarget->Release();
	this->swapChain->Release();
	this->deviceContext->Release();
	this->device->Release();
}
