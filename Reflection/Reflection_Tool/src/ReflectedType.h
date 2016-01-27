#pragma once

#include <vector>
#include "ReflectedVariable.h"

class ReflectedType
{
public:
	ReflectedType( const char* name );
	ReflectedType( const char* name, bool isPrimitive );
	ReflectedType( const ReflectedType& type );
	ReflectedType& operator=( const ReflectedType& type );
	~ReflectedType();
	
	bool AddParentType( const ReflectedType* parent );
	bool AddVariable( const char* name, const ReflectedType* type );
	void FinalizeType();
	
	const std::vector<const ReflectedType*>& GetParentTypes() const;
	const std::vector<ReflectedVariable*>& GetVariables() const;
	
	bool IsPrimitive() const;
	bool IsFinal() const;
	const char* GetName() const;
	
	bool operator==( const char* name ) const;
	bool HasParent( const ReflectedType* type ) const;
	bool HasVariable( const char* name ) const;
	bool HasVariable( const char* name, const ReflectedType* type ) const;
	
private:
	char* name;
	bool is_final;
	bool is_primitive;
	
	std::vector<const ReflectedType*> parents;
	std::vector<ReflectedVariable*> variables;
	
	
private:
	ReflectedType() = delete;
};