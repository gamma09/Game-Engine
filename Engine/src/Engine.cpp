#include "Engine.h"
#include <string.h>
#include <DebuggerSetup.h>
#include <GameAssert.h>
#include <mem.h>

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

	// TODO while window is open (requires information from the message pump)
	//while( glfwGetWindowParam( GLFW_OPENED ) != GL_FALSE )
	//{
	// moving
	Update();

	// rendering
	ClearBufferFunc();
	Draw();

	//TODO swap buffers
	//	glfwSwapBuffers();
	//}

	UnLoadContent();

	this->PostUnLoadContent();

	// TODO shut down OpenGL
	// glfwTerminate();
}

void Engine::PreInitialize()
{
	app = this;
	if( GLEW_OK != glewInit() )
	{
		out( "Failed to initialize GLEW\n" );
		return;
	}

	GameVerify( Mem_OK == Mem::initialize() );

	TemporaryHeap::Create();

	GameVerify( Mem_OK == Mem::createHeap( this->managerHeap, 4096, "Manager Heap" ) );
	GameVerify( Mem_OK == Mem::createHeap( this->materialHeap, 4096, "Material Heap" ) );

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

	// TODO do we need to set up opengl version major and minor?
	// info->majorVersion
	// info->minorVersion

	// TODO setup 
	// TODO glfwOpenWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
	// TODO glfwOpenWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );
	// TODO glfwOpenWindowHint( GLFW_FSAA_SAMPLES, info->samples );
	// TODO glfwOpenWindowHint( GLFW_STEREO, info->flags.stereo ? GL_TRUE : GL_FALSE );
	if( info->flags.fullscreen )
	{
		if( info->windowWidth == 0 || info->windowHeight == 0 )
		{
			// TODO GLFWvidmode mode;
			// TODO glfwGetDesktopMode( &mode );
			// TODO info->windowWidth = mode.Width;
			// TODO info->windowHeight = mode.Height;
		}
		// TODO glfwOpenWindow( info->windowWidth, info->windowHeight, 8, 8, 8, 0, 32, 0, GLFW_FULLSCREEN );
		// TODO glfwSwapInterval( info->flags.vsync );
	}
	else
	{
		// TODO if( !glfwOpenWindow( info->windowWidth, info->windowHeight, 8, 8, 8, 0, 32, 0, GLFW_WINDOW ) )
		{
			out( "Failed to open window\n" );
			return;
		}
	}

	// TODO glfwSetWindowTitle( info->title );
	// TODO glfwSetWindowSizeCallback( glfw_onResize );
	// TODO ( info->flags.cursor ? glfwEnable : glfwDisable )( GLFW_MOUSE_CURSOR );

	// TODO info->flags.stereo = glfwGetWindowParam( GLFW_STEREO );

#ifdef _DEBUG
	out( "VENDOR: %s\n", (char *) glGetString( GL_VENDOR ) );
	out( "VERSION: %s\n", (char *) glGetString( GL_VERSION ) );
	out( "RENDERER: %s\n", (char *) glGetString( GL_RENDERER ) );
#endif

	if( info->flags.stereo )
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

	GameVerify( Mem_OK == Mem::destroy() );
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
	info->minorVersion = 0;

	info->samples = 0;
	info->flags.all = 0;
	info->flags.cursor = 1;

#ifdef _DEBUG
	info->flags.debug = 1;
#endif

}

Engine::~Engine()
{

}








void Engine::onResize( int w, int h )
{
	info->windowWidth = w;
	info->windowHeight = h;
}

void Engine::onKey( int key, int action )
{
	key;//not used
	action;//not used
}

void Engine::onMouseButton( int button, int action )
{
	button;//not used
	action;//not used
}

void Engine::onMouseMove( int x, int y )
{
	x; /*not used*/
	y;/* not used*/
}

void Engine::onMouseWheel( int pos )
{
	pos; /*not used*/
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

void Engine::getMousePosition( int& x, int& y )
{
	x;
	y;
	// TODO glfwGetMousePos( &x, &y );
}




/* TODO void GLFWCALL Engine::glfw_onResize( int w, int h )
{
app->onResize( w, h );
} */


void Engine::setVsync( bool enable )
{
	info->flags.vsync = enable ? 1 : 0;
	// TODO glfwSwapInterval( info->flags.vsync );
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