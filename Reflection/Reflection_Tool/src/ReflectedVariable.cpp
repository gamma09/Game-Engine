#include <string.h>
#include <GameAssert.h>

#include "ReflectedVariable.h"
#include "ReflectedType.h"

ReflectedVariable::ReflectedVariable( const char* inVarName, ReflectedType* inVarType ) :
	varName( nullptr ),
	varType( inVarType )
{
	GameAssert( inVarName );
	GameAssert( inVarType );
	
	this->varName = new char[strlen( inVarName ) + 1];
	strcpy( this->varName, inVarName );
}

ReflectedVariable::ReflectedVariable( const ReflectedVariable& var ) :
	varName( new char[strlen( var.varName ) + 1] ),
	varType( var.varType )
{
	strcpy( this->varName, var.varName );
}

ReflectedVariable& ReflectedVariable::operator=( const ReflectedVariable& var )
{
	this->varName = new char[strlen( var.varName ) + 1];
	strcpy( this->varName, var.varName );
	this->varType = var.varType;
	return *this;
}

ReflectedVariable::~ReflectedVariable()
{
	delete this->varName;
	this->varName = nullptr;
	this->varType = nullptr;
}
	
const char* ReflectedVariable::GetName() const
{
	return this->varName;
}

const ReflectedType* ReflectedVariable::GetType() const
{
	return this->varType;
}
	
bool ReflectedVariable::operator==( const ReflectedVariable& var ) const
{
	return strcmp( this->varName, var.varName ) == 0 && *this->varType == *var.varType;
}

bool ReflectedVariable::operator==( const char* name ) const
{
	return strcmp( this->varName, name ) == 0;
}
