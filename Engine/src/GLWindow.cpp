#include <GameAssert.h>
#include "GLWindow.h"
#include "EngineMessages.h"

GLWindow::GLWindow( const char* inTitle, int inWidth, int inHeight ) :
	width( inWidth ),
	height( inHeight )
{
	GameAssert( inTitle != nullptr );
	unsigned int length = strlen( inTitle ) + 1;
	this->title = new char[length];
	strcpy_s( this->title, length, inTitle );

	this->hThread = CreateThread( NULL, 0, WindowThreadEntry, this, 0, NULL );
	GameAssert( this->hThread );
}

GLWindow::~GLWindow()
{
	GameVerify( PostMessage( this->hWindow, ENGINE_KILL_WINDOW, 0, 0 ) );
	WaitForSingleObject( this->hThread, INFINITE );

	CloseHandle( this->hThread );
	
	delete this->title;
}

DWORD WINAPI GLWindow::WindowThreadEntry( LPVOID lpParameter )
{
	WNDCLASS wc;
	wc.style = 0;
	wc.lpfnWndProc = WindowCallback;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetModuleHandle( NULL );
	wc.

	GLWindow* window = (GLWindow*) lpParameter;
	GameAssert( window );
	return window->Run();
}

DWORD GLWindow::Run()
{
	this->hWindow = CreateWindow(  )
}

	void MessagePump();
	static LRESULT CALLBACK WindowCallback( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );