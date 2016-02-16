#pragma once

#include "Reflection.h"

class Object
{
	REFLECTED_CLASS( Object )

protected:
	Object() { }
	Object( const Object& ) { }
	Object( Object&& ) { }
	Object& operator=( const Object& ) { return *this; }
	Object& operator=( Object&& ) { return *this; }

public:
	virtual ~Object() { }
};