#pragma once

#include "Component.h"

namespace GUI
{
	class Label : public Component
	{
	public:
		Label();
		Label( const GUI::Rectangle& rect );
		Label( const char* text );
		Label( const GUI::Rectangle& rect, const char* text );
		virtual ~Label();
	};
}