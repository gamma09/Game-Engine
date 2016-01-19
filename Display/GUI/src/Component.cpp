#pragma warning( push )
#pragma warning( disable : 4244 )

#include <FL/Fl_Widget.H>

#pragma warning( pop )

#include <GameAssert.h>
#include "Component.h"

namespace GUI
{

	Component::Component( Fl_Widget* fltk_widget ) :
		PCSNode(),
		fltk_component( fltk_widget )
	{
		GameAssert( fltk_widget );
	}

	Component::~Component()
	{
		if( nullptr != this->fltk_component )
		{
			delete this->fltk_component;
		}
	}

	Fl_Widget* Component::Get_FLTK_Component() const
	{
		return this->fltk_component;
	}

	void Component::Set_Visible( bool isVisible ) const
	{
		if( isVisible )
		{
			this->fltk_component->show();
		}
		else
		{
			this->fltk_component->hide();
		}
	}

	bool Component::Is_Visible() const
	{
		return 0 != this->fltk_component->visible();
	}

	void Component::Set_Active( bool isActive ) const
	{
		if( isActive )
		{
			this->fltk_component->activate();
		}
		else
		{
			this->fltk_component->deactivate();
		}
	}

	bool Component::Is_Active() const
	{
		return 0 != this->fltk_component->active();
	}

	void Component::Set_Tooltip( const char* tooltipText ) const
	{
		GameAssert( tooltipText );

		this->fltk_component->copy_tooltip( tooltipText );
		this->Repaint();
	}

	const char* Component::Get_Tooltip() const
	{
		return this->fltk_component->tooltip();
	}

	void Component::Set_Text( const char* text ) const
	{
		this->fltk_component->copy_label( text );
	}

	const char* Component::Get_Text() const
	{
		return this->fltk_component->label();
	}

	void Component::Set_Bounds( const Rectangle& rect ) const
	{
		this->fltk_component->resize( rect.x, rect.y, rect.width, rect.height );
		this->Repaint();
	}

	Rectangle Component::Get_Bounds() const
	{
		return Rectangle( this->fltk_component->x(), this->fltk_component->y(), this->fltk_component->w(), this->fltk_component->h() );
	}

	void Component::Repaint() const
	{
		this->fltk_component->redraw();
	}


}
