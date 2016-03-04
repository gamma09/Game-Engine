#pragma once

#include "LibrarySetup.h"
#include <GL\GL.h>

#include <cstdio>
#include <cstring>

class Heap;
struct AppInfo;

class Engine abstract
{
public:
	Engine( const char* windowName, const int Width,const int Height);
	virtual ~Engine();

	// required overloading
	virtual void Initialize() abstract;
	virtual void LoadContent() abstract;
	virtual void Update() abstract;
	virtual void Draw() abstract;
	virtual void UnLoadContent() abstract;
	virtual void onDebugMessage( GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message );

	// optional overloading
	virtual void ClearBufferFunc();

	void run();


	// private: --------------------------------------
private:
	// force to use the appropriate constructor
	Engine() = delete;

	// private functions
	void PreInitialize();
	void PreLoadContent();
	void PostUnLoadContent();

public:
	virtual void onResize(int w, int h);
	virtual void onKey(int key, int action);
	virtual void onMouseButton(int button, int action);
	virtual void onMouseMove(int x, int y);
	virtual void onMouseWheel(int pos);
	static void getMousePosition(int& x, int& y);

public:
	

protected:

	AppInfo* info;
	Heap* managerHeap;
	Heap* materialHeap;

	static Engine* app;

	// TODO code on resize equivalent in message pump
	//static void GLFWCALL glfw_onResize(int w, int h);

	void setVsync(bool enable);

	static void APIENTRY debug_callback( GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam );
};

