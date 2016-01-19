#pragma warning( push )
#pragma warning( disable : 4244 )

#include <FL/Fl_Input.H>

#pragma warning( pop )

#include <GameAssert.h>

#include "TextField.h"

TextField::TextField() :
	Component( new Fl_Input( 0, 0, 0, 0 ) )
{
	// Do nothing
}

TextField::TextField( const Rectangle& rect ) :
	Component( new Fl_Input( rect.x, rect.y, rect.width, rect.height ) )
{
	// Do nothing
}

TextField::TextField( const char* initialValue ) :
	Component( new Fl_Input( 0, 0, 0, 0 ) )
{
	GameAssert( initialValue );
	this->Set_Value( initialValue );
}

TextField::TextField( const Rectangle& rect, const char* initialValue ) :
	Component( new Fl_Input( rect.x, rect.y, rect.width, rect.height ) )
{
	GameAssert( initialValue );
	this->Set_Value( initialValue );
}

TextField::~TextField()
{
	// Do nothing
}

void TextField::Set_Value( const char* value ) const
{
	GameAssert( value );
	static_cast<Fl_Input*>( this->Get_FLTK_Component( ) )->value( value );
}

const char* TextField::Get_Value() const
{
	return static_cast<Fl_Input*>( this->Get_FLTK_Component() )->value();
}