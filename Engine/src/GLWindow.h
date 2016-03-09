#pragma once

#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <Windows.h>

#include <atomic>

struct AppInfo;

// This class creates a new thread for the window
class GLWindow
{
public:
	GLWindow( const AppInfo* info );
	~GLWindow();

	void PreDraw();
	void PostDraw();
	inline bool IsOpen() const { return this->isOpen; }

private:
	DWORD Run();
	void MessagePump();
	void CreateGLWindow();
	void CreateDeviceContext();
	void CreateRenderContext();
	static DWORD WINAPI WindowThreadEntry( LPVOID lpParameter );
	static LRESULT CALLBACK WindowCallback( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );


private:
	HWND hWindow;
	HGLRC hRenderContext;
	bool isOpen;

	HANDLE hThread;
	HDC hDeviceContext;

	const AppInfo* info;
	
	std::atomic<bool> ready;

private:
	GLWindow( const GLWindow& wnd ) = delete;
	GLWindow( GLWindow&& wnd ) = delete;
	GLWindow& operator=( const GLWindow& wnd ) = delete;
	GLWindow& operator=( GLWindow&& wnd ) = delete;
};