#include "Variable.h"

Variable::Variable( const char* inName, const BaseClass& inType, unsigned int inOffset )
:	type( inType ),
	offset( inOffset )
{
	GameAssert( inName );
	GameAssert( strlen( inName ) );
	
	size_t length = strlen( inName ) + 1;
	this->name = strncpy( new char[length], inName, length );
}

Variable::Variable( const Variable& var )
:	type( var.type ),
	offset( var.offset )
{
	GameAssert( var.name );
	GameAssert( strlen( var.name ) );
	
	size_t length = strlen( var.name ) + 1;
	this->name = strncpy( new char[length], var.name, length );
}

Variable::Variable( Variable&& var )
:	name( var.name ),
	type( var.type ),
	offset( var.offset )
{
	GameAssert( var.name );
	
	var.name = nullptr;
}

Variable::~Variable()
{
	if( this->name != nullptr )
	{
		delete this->name;
	}
}