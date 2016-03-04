#pragma once

#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <Windows.h>

// This class creates a new thread for the window
class GLWindow
{
public:
	GLWindow( const char* title, int width, int height );
	~GLWindow();

private:
	DWORD Run();
	void MessagePump();
	static DWORD WINAPI WindowThreadEntry( LPVOID lpParameter );
	static LRESULT CALLBACK WindowCallback( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );


private:
	HANDLE hThread;
	HWND hWindow;
	char* title;
	int width;
	int height;


private:
	GLWindow( const GLWindow& wnd ) = delete;
	GLWindow( GLWindow&& wnd ) = delete;
	GLWindow& operator=( const GLWindow& wnd ) = delete;
	GLWindow& operator=( GLWindow&& wnd ) = delete;
};