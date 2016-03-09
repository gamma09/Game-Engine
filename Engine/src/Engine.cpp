#include "Engine.h"
#include <string.h>
#include <DebuggerSetup.h>
#include <GameAssert.h>
#include <mem.h>

#include "GLWindow.h"
#include "GlobalHeaps.h"
#include "AppInfo.h"
#include "TextureMaterial.h"
#include "LightingTextureMaterial.h"
#include "WireframeMaterial.h"

#include "ActorManager.h"
#include "BoneManager.h"
#include "CameraManager.h"
#include "DirectionLightManager.h"
#include "KeyBindingManager.h"
#include "ModelBaseManager.h"
#include "ShaderManager.h"
#include "TextureManager.h"


Engine* Engine::app = 0;

//------------------------------------------------------------------
// Engine::Run()
//		This is the internal game loop that the engine runs on.
//------------------------------------------------------------------
void Engine::run()
{
	this->PreInitialize();

	Initialize();

	this->PreLoadContent();

	LoadContent();

	while( this->window->IsOpen() )
	{
		// moving
		Update();

		// rendering
		this->window->PreDraw();
		ClearBufferFunc();
		Draw();

		this->window->PostDraw();
	}

	UnLoadContent();

	this->PostUnLoadContent();
}

void Engine::PreInitialize()
{
	app = this;
	if( GLEW_OK != glewInit() )
	{
		out( "Failed to initialize GLEW\n" );
		return;
	}

	TemporaryHeap::Create();

	GameVerify( Mem_OK == Mem::createVariableBlockHeap( this->managerHeap, 4096 ) );
	GameVerify( Mem_OK == Mem::createVariableBlockHeap( this->materialHeap, 4096 ) );

	ShaderManager::Create( this->managerHeap, 4, 1 );
	ModelBaseManager::Create( this->managerHeap, 7, 1 );
	CameraManager::Create( this->managerHeap, 1, 1 );
	TextureManager::Create( this->managerHeap, 10, 3 );
	ActorManager::Create( this->managerHeap, 12, 1 );
	DirectionLightManager::Create( this->managerHeap, 1, 1 );
	KeyBindingManager::Create( this->managerHeap, 4, 2 );
	BoneManager::Create( this->managerHeap, 100, 10 );
}

void Engine::PreLoadContent()
{
	if( info->fullscreen )
	{
		if( info->windowWidth == 0 || info->windowHeight == 0 )
		{
			info->windowWidth = GetSystemMetrics( SM_CXSCREEN );
			info->windowHeight = GetSystemMetrics( SM_CYSCREEN );
		}
	}

	this->window = new GLWindow( this->info );

#ifdef _DEBUG
	out( "VENDOR: %s\n", (char *) glGetString( GL_VENDOR ) );
	out( "VERSION: %s\n", (char *) glGetString( GL_VERSION ) );
	out( "RENDERER: %s\n", (char *) glGetString( GL_RENDERER ) );
#endif

	if( info->stereo )
	{
		if( GLEW_VERSION_4_3 )
		{
			glDebugMessageCallback( debug_callback, this );
			glEnable( GL_DEBUG_OUTPUT_SYNCHRONOUS );
		}

#if GL_ARB_debug_output
		glDebugMessageCallbackARB( debug_callback, this );
		glEnable( GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB );
#endif
	}

	TextureManager::Instance()->Create_Default_Texture();

	WireframeMaterial::Create_Material( this->materialHeap );
	TextureMaterial::Create_Material( this->materialHeap );
	LightingTextureMaterial::Create_Material( this->materialHeap );
}

void Engine::PostUnLoadContent()
{
	TextureMaterial::Destroy_Material();
	WireframeMaterial::Destroy_Material();
	LightingTextureMaterial::Destroy_Material();

	KeyBindingManager::Destroy();
	DirectionLightManager::Destroy();
	ActorManager::Destroy();
	TextureManager::Destroy();
	CameraManager::Destroy();
	ModelBaseManager::Destroy();
	BoneManager::Destroy();
	ShaderManager::Destroy();

	GameVerify( Mem_OK == Mem::destroyHeap( this->managerHeap ) );
	GameVerify( Mem_OK == Mem::destroyHeap( this->materialHeap ) );

	TemporaryHeap::Destroy();

	delete this->window;
}

//------------------------------------------------------------------
// Engine::ClearBufferFunc()
// Allows user to change the way the clear buffer function works
//------------------------------------------------------------------
void Engine::ClearBufferFunc()
{
	const GLfloat grey[] ={ 0.250f, 0.25f, 0.25f, 1.0f };
	const GLfloat one = 1.0f;

	glClearBufferfv( GL_COLOR, 0, grey );
	glClearBufferfv( GL_DEPTH, 0, &one );
}

Engine::Engine( const char* windowName, const int Width, const int Height ) :
info( new AppInfo() )
{
	strcpy( info->title, windowName );
	info->windowWidth = Width;
	info->windowHeight = Height;

	info->majorVersion = 4;
	info->minorVersion = 3;

	info->samples = 0;
	info->cursor = true;
	info->fullscreen = false;
	info->stereo = false;
	info->vsync = true;

#ifdef _DEBUG
	info->debug = true;
#else
	info->debug = false;
#endif

}

Engine::~Engine()
{

}









void Engine::onDebugMessage( GLenum source,
							 GLenum type,
							 GLuint id,
							 GLenum severity,
							 GLsizei length,
							 const GLchar* message )
{
#ifdef _WIN32
	OutputDebugStringA( message );
	OutputDebugStringA( "\n" );

	type; //not used
	id; //not used
	severity; //not used
	length;//not used

	source; /*not used*/
#endif /* _WIN32 */
}





void APIENTRY Engine::debug_callback( GLenum source,
									  GLenum type,
									  GLuint id,
									  GLenum severity,
									  GLsizei length,
									  const GLchar* message,
									  const void* )
{
	Engine::app->onDebugMessage( source, type, id, severity, length, message );
}