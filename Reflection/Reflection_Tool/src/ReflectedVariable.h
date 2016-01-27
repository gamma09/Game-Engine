#pragma once

class ReflectedType;

class ReflectedVariable
{
public:
	ReflectedVariable( const char* varName, ReflectedType* varType );
	ReflectedVariable( const ReflectedVariable& var );
	ReflectedVariable& operator=( const ReflectedVariable& var );
	~ReflectedVariable();
	
	const char* GetName() const;
	const ReflectedType* GetType() const;
	
	bool operator==( const ReflectedVariable& var ) const;
	bool operator==( const char* name ) const;
	
private:
	char* varName;
	ReflectedType* varType;
};