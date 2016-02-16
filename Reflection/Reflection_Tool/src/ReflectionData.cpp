#include <utility>
#include <GameAssert.h>
#include "ReflectionData.h"

#define STRINGIFY( x ) #x
#define CONCAT( a, b ) STRINGIFY( a ## b )

#define ADD_PRIMITIVE_TYPE( container, typeName ) (container).emplace_back( STRINGIFY( typeName ), true );

#define ADD_PRIMITIVE_INT_TYPE( container, typeName ) \
	(container).emplace_back( STRINGIFY( typeName ), true );\
	(container).emplace_back( CONCAT( unsigned, typeName ), true );



ReflectionData::ReflectionData() :
	types()
{
	// Add primitive types - char, short, int, long, long long
	ADD_PRIMITIVE_INT_TYPE( this->types, char )
	ADD_PRIMITIVE_INT_TYPE( this->types, __int8 )
	ADD_PRIMITIVE_INT_TYPE( this->types, short )
	ADD_PRIMITIVE_INT_TYPE( this->types, short int )
	ADD_PRIMITIVE_INT_TYPE( this->types, __int16 )
	ADD_PRIMITIVE_INT_TYPE( this->types, int )
	ADD_PRIMITIVE_INT_TYPE( this->types, long )
	ADD_PRIMITIVE_INT_TYPE( this->types, __int32 )
	ADD_PRIMITIVE_INT_TYPE( this->types, long int )
	ADD_PRIMITIVE_INT_TYPE( this->types, long long )
	ADD_PRIMITIVE_INT_TYPE( this->types, __int64 )
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
				context.AddMessage( MSG_TYPE_LINK_ERROR, "Parent class %s of reflected class %s was not found.\n", type.GetParentType( ), type.GetName( ) );
			}
			else if( FindType( type.GetParentType() )->IsPrimitive() )
			{
				context.AddMessage( MSG_TYPE_LINK_ERROR, "Class %s may not have primitive type %s as its parent.\n", type.GetName( ), type.GetParentType( ) );
			}
		}
		
		// Make sure all of the variable types exist
		for( const ReflectedVariable& variable : type.GetVariables() )
		{
			if( FindType( variable.GetTypeName() ) == nullptr )
			{
				context.AddMessage( MSG_TYPE_LINK_ERROR, "Reflected type %s for variable %s in class %s was not found.\n", variable.GetTypeName(), variable.GetName(), type.GetName() );
			}
		}
	}
}
