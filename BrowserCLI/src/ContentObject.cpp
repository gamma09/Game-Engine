#include <string.h>
#include <GameAssert.h>

#include "ContentObject.h"

namespace CLI
{

	ContentObject::ContentObject()
		: name( nullptr )
	{
		// Do nothing
	}

	ContentObject::ContentObject( const char* inName )
	{
		GameAssert( inName != nullptr );

		unsigned int size = strlen( inName ) + 1;

		this->name = new char[size];
		strcpy_s( this->name, size, inName );
	}

	ContentObject::ContentObject( const ContentObject& obj )
	{
		if( obj.name == nullptr )
		{
			this->name = nullptr;
		}
		else
		{
			unsigned int size = strlen( obj.name ) + 1;

			this->name = new char[size];
			strcpy_s( this->name, size, obj.name );
		}
	}

	ContentObject& ContentObject::operator=( const ContentObject& obj )
	{
		if( this->name != nullptr )
		{
			delete this->name;
		}

		if( obj.name == nullptr )
		{
			this->name = nullptr;
		}
		else
		{
			unsigned int size = strlen( obj.name ) + 1;

			this->name = new char[size];
			strcpy_s( this->name, size, obj.name );
		}

		return *this;
	}

	ContentObject::~ContentObject()
	{
		if( this->name != nullptr )
		{
			delete this->name;
		}
	}

	void ContentObject::SetName( const char* inName )
	{
		GameAssert( inName != nullptr );

		if( this->name != nullptr )
		{
			delete this->name;
		}

		unsigned int size = strlen( inName ) + 1;

		this->name = new char[size];
		strcpy_s( this->name, size, inName );
	}

	const char* ContentObject::GetName() const
	{
		return this->name;
	}

}
