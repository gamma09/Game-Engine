#include <thread>
#include <GameAssert.h>
#include <GL\glew.h>
#include <GL\wglew.h>
#include "GLWindow.h"
#include "EngineMessages.h"
#include "AppInfo.h"

GLWindow::GLWindow( const AppInfo* windowInfo ) :
	info( windowInfo ),
	ready( false ),
	isOpen( true )
{
	this->hThread = CreateThread( NULL, 0, WindowThreadEntry, this, 0, NULL );
	GameAssert( this->hThread );

	while( !ready.load( std::memory_order_acquire ) )
		std::this_thread::yield();

	GameCheckFatal( wglMakeCurrent( this->hDeviceContext, this->hRenderContext ), "Could not make the render context current." );
}

GLWindow::~GLWindow()
{
	GameVerify( PostMessage( this->hWindow, ENGINE_KILL_WINDOW, 0, 0 ) );
	WaitForSingleObject( this->hThread, INFINITE );

	CloseHandle( this->hThread );
}

DWORD WINAPI GLWindow::WindowThreadEntry( LPVOID lpParameter )
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
	wc.lpszClassName = "GLWindow";
	RegisterClass( &wc );


	GLWindow* window = (GLWindow*) lpParameter;
	GameAssert( window );
	return window->Run();
}

void GLWindow::CreateGLWindow()
{
	DWORD exStyle = WS_EX_APPWINDOW;
	DWORD style = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE;
	RECT wa;

	if( this->info->fullscreen )
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
	rect.right = ( long ) this->info->windowWidth - 1;
	rect.top = (long) 0;
	rect.bottom = ( long ) this->info->windowHeight - 1;

	// Adjust according to window styles
	AdjustWindowRectEx( &rect, style, FALSE, exStyle );

	// Calculate width and height of full window
	int fullWidth = rect.right - rect.left + 1;
	int fullHeight = rect.bottom - rect.top + 1;

	// Adjust window position to working area (e.g. if the task bar is at
	// the top of the display). Fullscreen windows are always opened in
	// the upper left corner regardless of the desktop working area.
	if( this->info->fullscreen )
	{
		wa.left = wa.top = 0;
	}
	else
	{
		SystemParametersInfo( SPI_GETWORKAREA, 0, &wa, 0 );
	}


	this->hWindow = CreateWindowEx( exStyle, "GLWindow", "GLWindow", style, wa.left, wa.top, fullWidth, fullHeight, NULL, NULL, NULL, NULL );
	GameCheckFatal( this->hWindow, "Could not create window." );
}

void GLWindow::CreateDeviceContext()
{
	this->hDeviceContext = GetDC( this->hWindow );
	GameCheckFatal( this->hDeviceContext, "Could not retrieve window's device context." );
	int pixelAttrib[21];
	pixelAttrib[0] = WGL_SUPPORT_OPENGL_ARB;
	pixelAttrib[1] = TRUE;

	pixelAttrib[2] = WGL_DRAW_TO_WINDOW_ARB;
	pixelAttrib[3] = TRUE;

	pixelAttrib[4] = WGL_ACCELERATION_ARB;
	pixelAttrib[5] = WGL_FULL_ACCELERATION_ARB;

	pixelAttrib[6] = WGL_COLOR_BITS_ARB;
	pixelAttrib[7] = 24;

	pixelAttrib[8] = WGL_DEPTH_BITS_ARB;
	pixelAttrib[9] = 24;

	pixelAttrib[10] = WGL_DOUBLE_BUFFER_ARB;
	pixelAttrib[11] = TRUE;
	
	pixelAttrib[12] = WGL_SWAP_METHOD_ARB;
	pixelAttrib[13] = WGL_SWAP_EXCHANGE_ARB;

	pixelAttrib[14] = WGL_PIXEL_TYPE_ARB;
	pixelAttrib[15] = WGL_TYPE_RGBA_ARB;

	pixelAttrib[16] = WGL_SAMPLE_BUFFERS_ARB;
	pixelAttrib[17] = 1;

	pixelAttrib[18] = WGL_SAMPLES_ARB;
	pixelAttrib[19] = this->info->samples;

	pixelAttrib[20] = 0;

	int iPixelFormat;
	unsigned int formatCount;
	GameCheckFatal( wglChoosePixelFormatARB( this->hDeviceContext, pixelAttrib, NULL, 1, &iPixelFormat, &formatCount ), "Could not choose pixel format." );

	PIXELFORMATDESCRIPTOR pfd;
	SetPixelFormat( this->hDeviceContext, iPixelFormat, &pfd );
}

void GLWindow::CreateRenderContext()
{
	int attribs[40];
	int i = 0;
	if( this->info->majorVersion != 1 || this->info->minorVersion != 0 )
	{
		// Request an explicitly versioned context

		attribs[i++] = WGL_CONTEXT_MAJOR_VERSION_ARB;
		attribs[i++] = this->info->majorVersion;
		attribs[i++] = WGL_CONTEXT_MINOR_VERSION_ARB;
		attribs[i++] = this->info->minorVersion;
	}

	attribs[i++] = WGL_CONTEXT_FLAGS_ARB;
	if( info->debug )
	{
		attribs[i] = WGL_CONTEXT_DEBUG_BIT_ARB;
	}
	attribs[i++] |= WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB;

	attribs[i++] = WGL_CONTEXT_PROFILE_MASK_ARB;
	attribs[i++] = WGL_CONTEXT_CORE_PROFILE_BIT_ARB;

	attribs[i++] = 0;

	this->hRenderContext = wglCreateContextAttribsARB( this->hDeviceContext, NULL, attribs );
	GameCheckFatal( this->hRenderContext, "Could not create render context." );

	GameCheckFatal( wglMakeCurrent( this->hDeviceContext, this->hRenderContext ), "Could not make the render context current." );
	wglSwapIntervalEXT( this->info->vsync ? 1 : 0 );

	if( this->info->cursor )
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

	GameCheckFatal( wglMakeCurrent( this->hDeviceContext, NULL ), "Could not make the render context current." );
}

DWORD GLWindow::Run()
{
	CreateGLWindow();
	CreateDeviceContext();
	CreateRenderContext();

	this->ready.store( true, std::memory_order_release );

	SetWindowLongPtr( this->hWindow, GWLP_USERDATA, (LONG_PTR) this );

	MessagePump();

	this->isOpen = false;
}

void GLWindow::PreDraw()
{
	GameVerify( wglMakeCurrent( this->hDeviceContext, this->hRenderContext ) );
}

void GLWindow::PostDraw()
{
	SwapBuffers( this->hDeviceContext );
	GameVerify( wglMakeCurrent( this->hDeviceContext, NULL ) );
	
}

void GLWindow::MessagePump()
{
	MSG msg;
	while( GetMessage( &msg, this->hWindow, 0, 0 ) > 0 )
	{
		if( msg.message == WM_QUIT )
		{
			break;
		}

		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}
}

LRESULT CALLBACK GLWindow::WindowCallback( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	GLWindow* window = (GLWindow*) GetWindowLongPtr( hWnd, GWLP_USERDATA );

	switch( message )
	{
		case WM_CLOSE:
			wglMakeCurrent( NULL, NULL );
			wglDeleteContext( window->hRenderContext );
			ReleaseDC( window->hWindow, window->hDeviceContext );
			DestroyWindow( window->hWindow );
			break;

		case WM_DESTROY:
			PostQuitMessage( 0 );
			break;

		default:
			return DefWindowProc( hWnd, message, wParam, lParam );
	}

	return 0;
}