#pragma warning( push )
#pragma warning( disable : 4244 )
#include <FL\Fl_Window.H>
#pragma warning( pop )

#include "Window.h"

namespace GUI
{


	Window::Window( ) :
		Component( new Fl_Window( 0, 0 ) )
	{
		// Do nothing
	}

	Window::Window( const Rectangle& rect ) :
		Component( new Fl_Window( rect.x, rect.y, rect.width, rect.height ) )
	{
		// Do nothing
	}

	Window::Window( const char* title ) :
		Component( new Fl_Window( 0, 0, title ) )
	{
		// Do nothing
	}

	Window::Window( const Rectangle& rect, const char* title ) :
		Component( new Fl_Window( rect.x, rect.y, rect.width, rect.height, title ) )
	{
		// Do nothing
	}

	Window::~Window( )
	{
		// Do nothing
	}

}
