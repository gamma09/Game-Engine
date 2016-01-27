#include <stdio.h>
#include <string.h>
#include <GameAssert.h>

#include "ReflectedType.h"

ReflectedType::ReflectedType( const char* inName ) :
	name( nullptr ),
	is_final( false ),
	is_primitive( false ),
	parents(),
	variables()
{
	GameAssert( inName );
	this->name = new char[strlen( inName ) + 1];
	strcpy( this->name, inName );
}

ReflectedType::ReflectedType( const char* inName, bool isPrimitive ) :
	name( nullptr ),
	is_final( true ),
	is_primitive( true ),
	parents(),
	variables()
{
	GameAssert( inName );
	this->name = new char[strlen( inName ) + 1];
	strcpy( this->name, inName );
}

ReflectedType::ReflectedType( const ReflectedType& type ) :
	name( new char[strlen( type.name ) + 1] ),
	is_final( type.is_final ),
	is_primitive( type.is_primitive ),
	parents( type.parents ),
	variables( type.variables )
{
	strcpy( this->name, type.name );
}

ReflectedType& ReflectedType::operator=( const ReflectedType& type )
{
	this->name = new char[strlen( type.name ) + 1];
	strcpy( this->name, type.name );
	this->is_final = type.is_final;
	this->is_primitive = type.is_primitive;
	this->parents = type.parents;
	this->variables = type.variables;
	
	return *this;
}

ReflectedType::~ReflectedType()
{
	delete this->name;
}

bool ReflectedType::AddParentType( const ReflectedType* parent )
{
	GameAssert( !this->is_final );
	
	if( this == parent )
	{
		fprintf( stderr, "ERROR: Class %s cannot be its own parent.\n", this->name );
		return false;
	}
	
	if( this->HasParent( parent ) )
	{
		fprintf( stderr, "ERROR: Duplicate parent class %s declared for type %s\n", parent->name, this->name );
		return false;
	}
	
	this->parents.push_back( parent );
	return true;
}

bool ReflectedType::AddVariable( const char* name, const ReflectedType* type )
{
	GameAssert( !this->is_final );
	
	if( this->HasVariable( name ) )
	{
		fprintf( stderr, "ERROR: Duplicate variable %s in class %s\n", name, this->name );
		return false;
	}
	
	this->variables.push_back( new ReflectedVariable( name, type ) );
	return true;
}

void ReflectedType::FinalizeType()
{
	this->is_final = true;
}

const std::vector<const ReflectedType*>& ReflectedType::GetParentTypes() const
{
	return this->parents;
}

const std::vector<ReflectedVariable*>& ReflectedType::GetVariables() const
{
	return this->variables;
}

bool ReflectedType::IsPrimitive() const
{
	return this->is_primitive;
}

bool ReflectedType::IsFinal() const
{
	return this->is_final;
}

const char* ReflectedType::GetName() const
{
	return this->name;
}

bool ReflectedType::operator==( const char* name ) const
{
	return strcmp( this->name, name ) == 0;
}

bool ReflectedType::HasParent( const ReflectedType* type ) const
{
	for( unsigned int index = 0; index < this->parents.size(); ++index )
	{
		if( *this->parents[index] == *type )
		{
			return true;
		}
	}
	
	return false;
}

bool ReflectedType::HasVariable( const char* name ) const
{
	for( unsigned int index = 0; index < this->variables.size(); ++index )
	{
		if( *this->variables[index] == name )
		{
			return true;
		}
	}
	
	return false;
}

bool ReflectedType::HasVariable( const char* name, const ReflectedType* type ) const
{
	for( unsigned int index = 0; index < this->variables.size(); ++index )
	{
		if( *this->variables[index] == name && this->variables[index]->GetType() == type )
		{
			return true;
		}
	}
	
	return false;
}