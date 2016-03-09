#pragma once

#include "LibrarySetup.h"
#include <GL\GL.h>

#include <cstdio>
#include <cstring>

class Heap;
struct AppInfo;
class GLWindow;

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

	GLWindow* window;
	AppInfo* info;
	Heap* managerHeap;
	Heap* materialHeap;

	static Engine* app;

	static void APIENTRY debug_callback( GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam );
};

