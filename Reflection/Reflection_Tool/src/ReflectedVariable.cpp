#include <string.h>
#include <GameAssert.h>

#include "ReflectedVariable.h"
#include "ReflectedType.h"

ReflectedVariable::ReflectedVariable( const char* inVarName, const char* inVarType )
{
	GameAssert( inVarName );
	GameAssert( inVarType );
	
	this->varName = new char[strlen( inVarName ) + 1];
	strcpy( this->varName, inVarName );
	
	this->varType = new char[strlen( inVarType ) + 1];
	strcpy( this->varType, inVarType );
}

ReflectedVariable::ReflectedVariable( const ReflectedVariable& var )
{
	GameAssert( var.varName );
	GameAssert( var.varType );
	
	this->varName = new char[strlen( var.varName ) + 1];
	strcpy( this->varName, var.varName );
	
	this->varType = new char[strlen( var.varType ) + 1];
	strcpy( this->varType, var.varType );
}

ReflectedVariable::ReflectedVariable( ReflectedVariable&& var ) :
	varName( var.varName ),
	varType( var.varType )
{
	GameAssert( var.varName );
	GameAssert( var.varType );
	
	var.varName = nullptr;
	var.varType = nullptr;
}

ReflectedVariable& ReflectedVariable::operator=( const ReflectedVariable& var )
{
	GameAssert( this != &var );
	GameAssert( this->varName );
	GameAssert( this->varType );
	GameAssert( var.varName );
	GameAssert( var.varType );
	
	delete this->varName;
	delete this->varType;
	
	this->varName = new char[strlen( var.varName ) + 1];
	strcpy( this->varName, var.varName );
	this->varType = new char[strlen( var.varType ) + 1];
	strcpy( this->varType, var.varType );
	
	return *this;
}

ReflectedVariable& ReflectedVariable::operator=( ReflectedVariable&& var )
{
	GameAssert( this != &var );
	GameAssert( this->varName );
	GameAssert( this->varType );
	GameAssert( var.varName );
	GameAssert( var.varType );
	
	delete this->varName;
	delete this->varType;
	
	this->varName = var.varName;
	this->varType = var.varType;
	
	var.varName = nullptr;
	var.varType = nullptr;
	
	return *this;
}

ReflectedVariable::~ReflectedVariable()
{
	if( this->varName != nullptr )
	{
		delete this->varName;
		this->varName = nullptr;
	}
	
	if( this->varType != nullptr )
	{
		delete this->varType;
		this->varType = nullptr;
	}
}
	
const char* ReflectedVariable::GetName() const
{
	GameAssert( this->varName );
	
	return this->varName;
}

const char* ReflectedVariable::GetTypeName() const
{
	return this->varType;
}
	
bool ReflectedVariable::operator==( const ReflectedVariable& var ) const
{
	GameAssert( var.varName );
	GameAssert( var.varType );
	GameAssert( this->varName );
	GameAssert( this->varType );
	
	return strcmp( this->varName, var.varName ) == 0 && strcmp( this->varType, var.varType ) == 0;
}

bool ReflectedVariable::operator==( const char* name ) const
{
	GameAssert( this->varName );
	GameAssert( name );
	
	return strcmp( this->varName, name ) == 0;
}
