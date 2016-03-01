#pragma once

#include <Reflection.h>
#include <Object.h>
#include <Vect.h>

struct ReflVect : public Object
{
	REFLECTED_CLASS( ReflVect );

	REFLECTED( float, _x );
	REFLECTED( float, _y );
	REFLECTED( float, _z );
	REFLECTED( float, _w );





	ReflVect() : Object() { }
	ReflVect( const Vect& vect ) : Object(), _x( vect[X] ), _y( vect[Y] ), _z( vect[Z] ), _w( vect[W] ) { }
	ReflVect( const ReflVect& v ) : Object( v ), _x( v._x ), _y( v._y ), _z( v._z ), _w( v._w ) { }
	ReflVect( ReflVect&& v ) : Object( v ), _x( v._x ), _y( v._y ), _z( v._z ), _w( v._w ) { }
	
	ReflVect& operator=( const ReflVect& v )
	{
		Object::operator=( v );
		this->_x = v._x;
		this->_y = v._y;
		this->_z = v._z;
		this->_w = v._w;

		return *this;
	}

	ReflVect& operator=( ReflVect&& v )
	{
		Object::operator=( v );
		this->_x = v._x;
		this->_y = v._y;
		this->_z = v._z;
		this->_w = v._w;

		return *this;
	}

	ReflVect& operator=( const Vect& vect )
	{
		this->_x = vect[X];
		this->_y = vect[Y];
		this->_z = vect[Z];
		this->_w = vect[W];

		return *this;
	}

	virtual ~ReflVect() { }

	operator Vect() const
	{
		return Vect( this->_x, this->_y, this->_z, this->_w );
	}
};
