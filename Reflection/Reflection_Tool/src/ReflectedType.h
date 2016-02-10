#pragma once

#include <vector>
#include "ReflectedVariable.h"
#include "FeedbackContext.h"

class ReflectedType
{
public:
	ReflectedType( const char* name );
	ReflectedType( const char* name, bool isPrimitive );
	ReflectedType( const ReflectedType& type );
	ReflectedType( ReflectedType&& type );
	ReflectedType& operator=( const ReflectedType& type );
	ReflectedType& operator=( ReflectedType&& type );
	~ReflectedType();
	
	void AddParentType( const char* parent, FeedbackContext& context );
	void AddVariable( const char* name, const char* type, FeedbackContext& context );
	void FinalizeType();
	
	const std::vector<char*>& GetParentTypes() const;
	const std::vector<ReflectedVariable>& GetVariables() const;
	
	bool IsPrimitive() const;
	bool IsFinal() const;
	const char* GetName() const;
	
	bool operator==( const char* name ) const;
	bool operator<( const ReflectedType& other ) const;
	
	bool HasParent( const char* type ) const;
	bool HasVariable( const char* name ) const;
	bool HasVariable( const char* name, const char* type ) const;
	
private:
	char* name;
	bool is_final;
	bool is_primitive;
	
	std::vector<char*> parents;
	std::vector<ReflectedVariable> variables;
	
	
private:
	ReflectedType() = delete;
};
