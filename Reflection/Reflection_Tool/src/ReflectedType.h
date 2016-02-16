#pragma once

#include <vector>
#include "ReflectedVariable.h"
#include "FeedbackContext.h"

class ReflectedType
{
public:
	ReflectedType( const char* name, bool isPrimitive = false );
	ReflectedType( const ReflectedType& type );
	ReflectedType( ReflectedType&& type );
	ReflectedType& operator=( const ReflectedType& type );
	ReflectedType& operator=( ReflectedType&& type );
	~ReflectedType();
	
	void SetParentType( const char* parent, FeedbackContext& context );
	void AddVariable( const char* name, const char* type, FeedbackContext& context );
	void SetFile( const char* file, FeedbackContext& context );
	void FinalizeType();
	
	const char* GetParentType() const;
	const std::vector<ReflectedVariable>& GetVariables() const;
	const char* GetFile() const;
	
	bool IsPrimitive() const;
	bool IsFinal() const;
	const char* GetName() const;
	
	bool operator==( const char* name ) const;
	bool operator<( const ReflectedType& other ) const;
	
	bool HasVariable( const char* name ) const;
	bool HasVariable( const char* name, const char* type ) const;
	
private:
	char* name;
	char* file;
	char* parent;

	bool is_final;
	bool is_primitive;
	
	std::vector<ReflectedVariable> variables;
	
	
private:
	ReflectedType() = delete;
};
