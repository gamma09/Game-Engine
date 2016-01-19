#include "Rectangle.h"

Rectangle::Rectangle() :
	x( 0 ),
	y( 0 ),
	width( 0 ),
	height( 0 )
{
	// Do nothing
}

Rectangle::Rectangle( int inX, int inY, int inWidth, int inHeight ) :
	x( inX ),
	y( inY ),
	width( inWidth ),
	height( inHeight )
{
	// Do nothing
}

Rectangle::Rectangle( const Rectangle& rect ) :
	x( rect.x ),
	y( rect.y ),
	width( rect.width ),
	height( rect.height )
{
	// Do nothing
}

Rectangle& Rectangle::operator=( const Rectangle& rect )
{
	this->x = rect.x;
	this->y = rect.y;
	this->width = rect.width;
	this->height = rect.height;

	return *this;
}

Rectangle::~Rectangle()
{
	// Do nothing
}