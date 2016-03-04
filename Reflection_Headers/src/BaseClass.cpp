#include <string.h>
#include <GameAssert.h>
#include "BaseClass.h"


BaseClass::BaseClass( const char* inName, const BaseClass* inParent, unsigned int inSize ) :
	parent( inParent ),
	size( inSize ),
	variableHead( nullptr )
{
	GameAssert( inName );
	GameAssert( strlen( inName ) );
	
	size_t length = strlen( inName ) + 1;
	this->name = strncpy( new char[length], inName, length );
}

BaseClass::~BaseClass()
{
	delete this->name;
}