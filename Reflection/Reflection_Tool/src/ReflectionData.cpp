#include <utility>
#include <string>
#include <GameAssert.h>
#include "ReflectionData.h"

#define STRINGIFY( x ) #x

#define ADD_PRIMITIVE_TYPE( container, typeName ) (container).emplace_back( STRINGIFY( typeName ), true );

#define ADD_PRIMITIVE_INT_TYPE( container, typeName ) \
	(container).emplace_back( STRINGIFY( typeName ), true );\
	(container).emplace_back( (std::string("unsigned ") + STRINGIFY( typeName )).c_str(), true );



ReflectionData::ReflectionData() :
	types()
{
	// Add primitive types - char, short, int, long, long long
	ADD_PRIMITIVE_INT_TYPE( this->types, char )
	ADD_PRIMITIVE_INT_TYPE( this->types, short )
	ADD_PRIMITIVE_INT_TYPE( this->types, int )
	ADD_PRIMITIVE_INT_TYPE( this->types, long )
	ADD_PRIMITIVE_INT_TYPE( this->types, long long )
	ADD_PRIMITIVE_TYPE( this->types, float )
	ADD_PRIMITIVE_TYPE( this->types, double )
	ADD_PRIMITIVE_TYPE( this->types, long double )
	ADD_PRIMITIVE_TYPE( this->types, bool )
}

ReflectionData::ReflectionData( const ReflectionData& data ) :
	types( data.types )
{
	// Do nothing
}

ReflectionData::ReflectionData( ReflectionData&& data ) :
	types( std::move( data.types ) )
{
	// Do nothing
}

ReflectionData& ReflectionData::operator=( const ReflectionData& data )
{
	GameAssert( this != &data );
	
	this->types.clear();
	this->types = data.types;
	
	return *this;
}

ReflectionData& ReflectionData::operator=( ReflectionData&& data )
{
	GameAssert( this != &data );
	
	this->types.clear();
	this->types = std::move( data.types );
	data.types.clear();
	
	return *this;
}

ReflectionData::~ReflectionData()
{
	// Do nothing
}
	
ReflectedType* ReflectionData::AddType( const char* name, FeedbackContext& context )
{
	GameAssert( name );
	
	if( strcmp( name, "" ) == 0 )
	{
		context.AddMessage( MSG_TYPE_ERROR, "Reflected class type name missing.\n" );
		return nullptr;
	}
	else
	{
		ReflectedType* existingType = FindType( name );
		if( existingType )
		{
			return existingType;
		}
		else
		{
			return &*( this->types.emplace( this->types.begin(), name ) );
		}
	}
}

ReflectedType* ReflectionData::FindType( const char* name )
{
	for( ReflectedType& type : this->types )
	{
		if( type == name )
		{
			return &type;
		}
	}

	return nullptr;
}

const ReflectedType* ReflectionData::FindType( const char* name ) const
{
	for( const ReflectedType& type : this->types )
	{
		if( type == name )
		{
			return &type;
		}
	}

	return nullptr;
}

const std::list<ReflectedType>& ReflectionData::GetTypes() const
{
	return this->types;
}

void ReflectionData::CheckVariableTypeLinks( FeedbackContext& context ) const
{
	for( const ReflectedType& type : this->types )
	{
		// Make sure the parent exists
		if( type.GetParentType( ) != nullptr )
		{
			if( FindType( type.GetParentType() ) == nullptr )
			{
				std::string message = std::string("Parent class ") + type.GetParentType() + " of reflected class " + type.GetName() + " was not found.";
				context.AddMessage( MSG_TYPE_LINK_ERROR, message.c_str() );
			}
			else if( FindType( type.GetParentType() )->IsPrimitive() )
			{
				std::string message = std::string( "Class " ) + type.GetName() + " may not have primitive type " + type.GetParentType() + " as its parent.";
				context.AddMessage( MSG_TYPE_LINK_ERROR, message.c_str() );
			}
		}
		
		// Make sure all of the variable types exist
		for( const ReflectedVariable& variable : type.GetVariables() )
		{
			if( FindType( variable.GetTypeName() ) == nullptr )
			{
				std::string message = std::string( "Reflected type " ) + variable.GetTypeName() + " for variable " + variable.GetName() + " in class " + type.GetName() + " was not found.";
				context.AddMessage( MSG_TYPE_LINK_ERROR, message.c_str());
			}
		}
	}
}
