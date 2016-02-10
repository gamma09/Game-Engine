#pragma once

class ReflectedType;

class ReflectedVariable
{
public:
	ReflectedVariable( const char* varName, const char* varType );
	ReflectedVariable( const ReflectedVariable& var );
	ReflectedVariable( ReflectedVariable&& var );
	ReflectedVariable& operator=( const ReflectedVariable& var );
	ReflectedVariable& operator=( ReflectedVariable&& var );
	~ReflectedVariable();
	
	const char* GetName() const;
	const char* GetTypeName() const;
	
	bool operator==( const ReflectedVariable& var ) const;
	bool operator==( const char* name ) const;
	
private:
	char* varName;
	char* varType;
};
