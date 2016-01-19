#pragma once

#include "Component.h"

namespace GUI
{
	class TextField : public Component
	{
	public:
		TextField();
		TextField( const GUI::Rectangle& rect );
		TextField( const char* initialValue );
		TextField( const GUI::Rectangle& rect, const char* initialValue );
		virtual ~TextField();

		void Set_Value( const char* value ) const;
		const char* Get_Value() const;
	};
}