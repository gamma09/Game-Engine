#pragma once

#include <set>
#include "ReflectedType.h"

class ReflectionData
{
public:

	ReflectionData();
	ReflectionData( const ReflectionData& data );
	ReflectionData( ReflectionData&& data );
	ReflectionData& operator=( const ReflectionData& data );
	ReflectionData& operator=( ReflectionData&& data );
	~ReflectionData();
	
	ReflectedType* AddType( const char* name, FeedbackContext& context );
	ReflectedType* FindType( const char* name );
	const std::set<ReflectedType>& GetTypes() const;
	void CheckVariableTypeLinks( FeedbackContext& context ) const;
	
private:
	std::set<ReflectedType> types;
};
