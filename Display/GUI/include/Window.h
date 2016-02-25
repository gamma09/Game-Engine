#pragma once

#include "Component.h"

namespace GUI
{
	class Window : public Component
	{
	public:
		Window( );
		Window( const GUI::Rectangle& rect );
		Window( const char* title );
		Window( const GUI::Rectangle& rect, const char* title );
		virtual ~Window( );
	};
}