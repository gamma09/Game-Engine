#pragma warning( push )
#pragma warning( disable : 4244 )
#include <FL/Fl_Box.H>
#pragma warning( pop )

#include "Label.h"

Label::Label() :
	Component( new Fl_Box( 0, 0, 0, 0 ) )
{
	// Do nothing
}
	
Label::Label( const Rectangle& rect ) :
	Component( new Fl_Box( rect.x, rect.y, rect.width, rect.height ) )
{
	// Do nothing
}

Label::Label( const char* text ) :
	Component( new Fl_Box( 0, 0, 0, 0 ) )
{
	this->Set_Text( text );
}

Label::Label( const Rectangle& rect, const char* text ) :
	Component( new Fl_Box( rect.x, rect.y, rect.width, rect.height ) )
{
	this->Set_Text( text );
}

Label::~Label()
{
	// Do nothing
}