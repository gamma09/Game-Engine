#include "Reflection.h"

class Y
{
REFLECTED_CLASS( Y )

	REFLECTED( int, a )
REFLECTED( float, b )

	double d;

	Y() { }
	void Foo() { }
};

class Z
{
	REFLECTED_CLASS( Z )
	REFLECTED( Y, y )
	REFLECTED( char, c )
};

class X : public Y, public Z
{
	REFLECTED_CLASS( X, Y, Z )
	REFLECTED( short, s )
	REFLECTED( double, rd )
};

int main( int argc, char** argv )
{
	int i = 3;
	i++;
}