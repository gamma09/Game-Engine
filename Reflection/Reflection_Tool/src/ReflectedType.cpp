#include <string.h>
#include <utility>
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
	is_final( type.is_final ),
	is_primitive( type.is_primitive ),
	parents(),
	variables( type.variables )
{
	GameAssert( type.name );
	
	for( const char* parent : type.parents )
	{
		char* copiedParent = new char[strlen( parent ) + 1];
		strcpy( copiedParent, parent );
		this->parents.push_back( copiedParent );
	}
	
	this->name = new char[strlen( type.name ) + 1];
	strcpy( this->name, type.name );
}

ReflectedType::ReflectedType( ReflectedType&& type ) :
	name( type.name ),
	is_final( type.is_final ),
	is_primitive( type.is_primitive ),
	parents( std::move( type.parents ) ),
	variables( std::move( type.variables ) )
{
	GameAssert( type.name );
	
	type.name = nullptr;
	type.parents.clear();
}

ReflectedType& ReflectedType::operator=( const ReflectedType& type )
{
	GameAssert( this != &type );
	GameAssert( type.name );
	
	this->name = new char[strlen( type.name ) + 1];
	strcpy( this->name, type.name );
	this->is_final = type.is_final;
	this->is_primitive = type.is_primitive;
	for( char* parent : this->parents )
	{
		delete parent;
	}
	
	this->parents.clear();
	
	for( const char* parent : type.parents )
	{
		char* copiedParent = new char[strlen(parent) + 1];
		strcpy( copiedParent, parent );
		this->parents.push_back( copiedParent );
	}
	
	this->parents = type.parents;
	this->variables = type.variables;
	
	return *this;
}

ReflectedType& ReflectedType::operator=( ReflectedType&& type )
{
	GameAssert( this != &type );
	GameAssert( type.name );
	
	this->name = type.name;
	type.name = nullptr;
	
	this->is_final = type.is_final;
	this->is_primitive = type.is_primitive;
	this->parents = std::move( type.parents );
	type.parents.clear();
	this->variables = std::move( type.variables );
	
	return *this;
}

ReflectedType::~ReflectedType()
{
	if( this->name != nullptr )
	{
		delete this->name;
		this->name = nullptr;
	}
	
	for( char* parent : this->parents )
	{
		delete parent;
	}
	
	this->parents.clear();
}

void ReflectedType::AddParentType( const char* parent, FeedbackContext& context )
{
	GameAssert( this->name );
	GameAssert( parent );
	GameAssert( !this->is_final );
	
	if( strcmp( this->name, parent ) == 0 )
	{
		context.AddMessage( MessageType::MSG_TYPE_ERROR, "Class %s cannot be its own parent.\n", this->name );
	}
	else if( this->HasParent( parent ) )
	{
		context.AddMessage( MessageType::MSG_TYPE_ERROR, "Duplicate parent class %s declared for type %s.\n", parent, this->name );
	}
	else
	{
		char* copiedParent = new char[strlen( parent ) + 1];
		strcpy( copiedParent, parent );
		this->parents.push_back( copiedParent );
	}
}

void ReflectedType::AddVariable( const char* inName, const char* type, FeedbackContext& context )
{
	GameAssert( this->name );
	GameAssert( inName );
	GameAssert( type );
	
	if( strlen( inName ) == 0 )
	{
		context.AddMessage( MessageType::MSG_TYPE_ERROR, "Variable has no name.\n" );
	}
	else if( strlen( type ) == 0 )
	{
		context.AddMessage( MessageType::MSG_TYPE_ERROR, "Variable %s has no type.\n", inName );
	}
	else if( this->HasVariable( inName ) && !this->HasVariable( inName, type ) )
	{
		context.AddMessage( MessageType::MSG_TYPE_ERROR, "Variable %s in class %s is declared multiple times with different types.\n", inName, this->name );
	}
	else if( !this->HasVariable( inName, type ) && this->is_final )
	{
		context.AddMessage( MessageType::MSG_TYPE_ERROR, "Reflected class %s defined multiple times with different member variables.\n", this->name );
	}
	else
	{
		this->variables.emplace_back( name, type );
	}
}

void ReflectedType::FinalizeType()
{
	GameAssert( this->name );
	
	this->is_final = true;
}

const std::vector<char*>& ReflectedType::GetParentTypes() const
{
	GameAssert( this->name );
	
	return this->parents;
}

const std::vector<ReflectedVariable>& ReflectedType::GetVariables() const
{
	GameAssert( this->name );
	
	return this->variables;
}

bool ReflectedType::IsPrimitive() const
{
	GameAssert( this->name );
	
	return this->is_primitive;
}

bool ReflectedType::IsFinal() const
{
	GameAssert( this->name );
	
	return this->is_final;
}

const char* ReflectedType::GetName() const
{
	GameAssert( this->name );
	
	return this->name;
}

bool ReflectedType::operator==( const char* inName ) const
{
	GameAssert( inName );
	GameAssert( this->name );
	
	return strcmp( this->name, inName ) == 0;
}

bool ReflectedType::operator<( const ReflectedType& other ) const
{
	GameAssert( this->name );
	GameAssert( other.name );
	
	return strcmp( this->name, other.name ) < 0;
}

bool ReflectedType::HasParent( const char* type ) const
{
	GameAssert( type );
	GameAssert( this->name );
	
	for( const char* parent : this->parents )
	{
		if( strcmp( parent, type ) == 0 )
		{
			return true;
		}
	}
	
	return false;
}

bool ReflectedType::HasVariable( const char* inName ) const
{
	GameAssert( inName );
	GameAssert( this->name );
	
	for( const ReflectedVariable& variable : this->variables )
	{
		if( variable == inName )
		{
			return true;
		}
	}
	
	return false;
}

bool ReflectedType::HasVariable( const char* inName, const char* type ) const
{
	GameAssert( inName );
	GameAssert( type );
	GameAssert( this->name );
	
	for( const ReflectedVariable& variable : this->variables )
	{
		if( variable == inName && strcmp( variable->GetTypeName(), type ) == 0 )
		{
			return true;
		}
	}
	
	return false;
}
