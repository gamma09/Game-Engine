#include <utility>
#include "ReflectionData.h"

#define STRINGIFY( x ) #x
#define CONCAT( a, b ) STRINGIFY( a ## b )

#define ADD_PRIMITIVE_TYPE( container, typeName ) (container).emplace( STRINGIFY( typeName ), true );

#define ADD_PRIMITIVE_INT_TYPE( container, typeName ) \
	(container).emplace( STRINGIFY( typeName ), true );\
	(container).emplace( CONCAT( unsigned, typeName ), true );



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
		return &*( this->types.emplace( name ).first );
	}
}

ReflectedType* ReflectionData::FindType( const char* name )
{
	std::set<ReflectedType>::iterator result = this->types.find( ReflectedType( name ) );
	if ( result == this->types.end() )
	{
		return nullptr;
	}
	else
	{
		return &*result;
	}
}

const std::set<ReflectedType>& ReflectionData::GetTypes() const
{
	return this->types;
}

void ReflectionData::CheckVariableTypeLinks( FeedbackContext& context ) const
{
	for( const ReflectedType& type : this->types )
	{
		// Make sure all of the parents exist
		for( const char* parentType : type.GetParentTypes() )
		{
			if( FindType( parentType ) == nullptr )
			{
				context.AddMessage( MSG_TYPE_LINK_ERROR, "Parent class %s of reflected class %s was not found.\n", parentType, type.GetName() );
			}
			else if ( FindType( parentType )->IsPrimitive() )
			{
				context.AddMessage( MSG_TYPE_LINK_ERROR, "Class %s may not have primitive type %s as its parent.\n", type.GetName(), parentType );
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
