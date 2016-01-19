#pragma once

#include <PCSNode.h>
#include "Rectangle.h"

class Fl_Widget;

namespace GUI
{
	// This class is abstract
	class Component : public PCSNode
	{
	protected:
		Component( Fl_Widget* fltk_widget );

		Fl_Widget* Get_FLTK_Component() const;

	public:
		virtual ~Component();

		void Set_Visible( bool isVisible ) const;
		bool Is_Visible() const;

		void Set_Active( bool isActive ) const;
		bool Is_Active() const;

		void Set_Tooltip( const char* tooltipText ) const;
		const char* Get_Tooltip() const;

		void Set_Text( const char* text ) const;
		const char* Get_Text() const;

		void Set_Bounds( const GUI::Rectangle& rect ) const;
		GUI::Rectangle Get_Bounds() const;

		void Repaint() const;

	private:

		Fl_Widget* fltk_component;
	};
}