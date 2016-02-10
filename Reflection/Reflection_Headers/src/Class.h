#pragma once

#include "BaseClass.h"

template< typename T >
class Class : public BaseClass
{
private:
	Class( const char* name ) : BaseClass( name ) { }
	virtual ~Class() { }
	
	Class( Class&& type ) = delete;
	Class& operator=( Class&& type ) = delete;
	Class( const Class& type ) = delete;
	Class& operator=( const Class& type ) = delete;
};