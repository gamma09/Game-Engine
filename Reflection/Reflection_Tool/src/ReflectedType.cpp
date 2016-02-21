#include <string.h>
#include <utility>
#include <GameAssert.h>

#include "ReflectedType.h"

ReflectedType::ReflectedType( const char* inName, bool isPrimitive ) :
	name( nullptr ),
	is_final( isPrimitive ),
	is_primitive( isPrimitive ),
	parent( nullptr ),
	variables(),
	file( nullptr )
{
	GameAssert( inName );
	
	this->name = new char[strlen( inName ) + 1];
	strcpy( this->name, inName );
}

ReflectedType::ReflectedType( const ReflectedType& type ) :
	is_final( type.is_final ),
	is_primitive( type.is_primitive ),
	variables( type.variables )
{
	GameAssert( type.name );
	
	if( type.parent != nullptr )
	{
		this->parent = new char[strlen( type.parent ) + 1];
		strcpy( this->parent, type.parent );
	}
	else
	{
		this->parent = nullptr;
	}

	if( type.file != nullptr )
	{
		this->file = new char[strlen( type.file ) + 1];
		strcpy( this->file, type.file );
	}
	else
	{
		this->file = nullptr;
	}
	
	this->name = new char[strlen( type.name ) + 1];
	strcpy( this->name, type.name );
}

ReflectedType::ReflectedType( ReflectedType&& type ) :
	name( type.name ),
	is_final( type.is_final ),
	is_primitive( type.is_primitive ),
	parent( type.parent ),
	file( type.file ),
	variables( std::move( type.variables ) )
{
	GameAssert( type.name );
	
	type.name = nullptr;
	type.parent = nullptr;
	type.file = nullptr;
}

ReflectedType& ReflectedType::operator=( const ReflectedType& type )
{
	GameAssert( this != &type );
	GameAssert( type.name );
	
	if( this->name != nullptr ) delete this->name;
	if( this->file != nullptr ) delete this->file;
	if( this->parent != nullptr ) delete this->parent;

	this->name = new char[strlen( type.name ) + 1];
	strcpy( this->name, type.name );
	this->is_final = type.is_final;
	this->is_primitive = type.is_primitive;

	if( type.parent != nullptr )
	{
		this->parent = new char[strlen( type.parent ) + 1];
		strcpy( this->parent, type.parent );
	}
	else
	{
		this->parent = nullptr;
	}

	if( type.file != nullptr )
	{
		this->file = new char[strlen( type.file ) + 1];
		strcpy( this->file, type.file );
	}
	else
	{
		this->file = nullptr;
	}

	this->variables = type.variables;
	
	return *this;
}

ReflectedType& ReflectedType::operator=( ReflectedType&& type )
{
	GameAssert( this != &type );
	GameAssert( type.name );
	
	if( this->name != nullptr ) delete this->name;
	if( this->file != nullptr ) delete this->file;
	if( this->parent != nullptr ) delete this->parent;

	this->name = type.name;
	type.name = nullptr;

	this->parent = type.parent;
	type.parent = nullptr;

	this->file = type.file;
	type.file = nullptr;
	
	this->is_final = type.is_final;
	this->is_primitive = type.is_primitive;

	this->variables = std::move( type.variables );
	
	return *this;
}

ReflectedType::~ReflectedType()
{
	if( this->name != nullptr )
	{
		delete this->name;
	}

	if( this->parent != nullptr )
	{
		delete this->parent;
	}

	if( this->file != nullptr )
	{
		delete this->file;
	}
}

void ReflectedType::SetParentType( const char* parent, FeedbackContext& context )
{
	GameAssert( this->name );
	GameAssert( parent );
	GameAssert( !this->is_final );
	GameAssert( this->parent == nullptr );
	
	if( strcmp( this->name, parent ) == 0 )
	{
		std::string message = std::string( "Class " ) + this->name + " cannot be its own parent.";
		context.AddMessage( MessageType::MSG_TYPE_ERROR, message.c_str() );
	}
	else
	{
		this->parent = new char[strlen( parent ) + 1];
		strcpy( this->parent, parent );
	}
}

void ReflectedType::AddVariable( const char* inName, const char* type, FeedbackContext& context )
{
	GameAssert( this->name );
	GameAssert( inName );
	GameAssert( type );
	
	if( strlen( inName ) == 0 )
	{
		context.AddMessage( MessageType::MSG_TYPE_ERROR, "Variable has no name." );
	}
	else if( strlen( type ) == 0 )
	{
		context.AddMessage( MessageType::MSG_TYPE_ERROR, (std::string("Variable ") + inName + " has no type.").c_str() );
	}
	else if( this->HasVariable( inName ) && !this->HasVariable( inName, type ) )
	{
		context.AddMessage( MessageType::MSG_TYPE_ERROR, (std::string("Variable ") + inName + " in class " + this->name + " is declared multiple times with different types.").c_str() );
	}
	else if( !this->HasVariable( inName, type ) && this->is_final )
	{
		context.AddMessage( MessageType::MSG_TYPE_ERROR, (std::string("Reflected class ") + this->name + " defined multiple times with different member variables.").c_str() );
	}
	else
	{
		this->variables.emplace_back( name, type );
	}
}

void ReflectedType::SetFile( const char* inFile, FeedbackContext& context )
{
	GameAssert( this->name );
	GameAssert( inFile );
	GameAssert( this->file == nullptr );

	if( strlen( inFile ) == 0 )
	{
		context.AddMessage( MessageType::MSG_TYPE_ERROR, "Filename is the empty string." );
	}
	else
	{
		this->file = new char[strlen( inFile ) + 1];
		strcpy( this->file, inFile );
	}
}

void ReflectedType::FinalizeType()
{
	GameAssert( this->name );
	
	this->is_final = true;
}

const char* ReflectedType::GetParentType() const
{
	GameAssert( this->name );
	
	return this->parent;
}

const std::vector<ReflectedVariable>& ReflectedType::GetVariables() const
{
	GameAssert( this->name );
	
	return this->variables;
}

const char* ReflectedType::GetFile() const
{
	GameAssert( this->name );

	return this->file;
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
		if( variable == inName && strcmp( variable.GetTypeName(), type ) == 0 )
		{
			return true;
		}
	}
	
	return false;
}
