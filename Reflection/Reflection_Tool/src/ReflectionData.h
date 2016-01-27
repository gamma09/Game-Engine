#pragma once

#include "ReflectedType.h"

class ReflectionData
{
public:
	ReflectionData();
	ReflectionData( const ReflectionData& data );
	ReflectionData& operator=( const ReflectionData& data );
	~ReflectionData();
	
private:
	std::vector<ReflectedType*> types;
};