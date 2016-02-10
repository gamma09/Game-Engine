#pragma once

template< typename T >
class ReflectedType
{
private:
	ReflectedType();
	~ReflectedType();
	
	ReflectedType( ReflectedType&& type ) = delete;
	ReflectedType& operator=( ReflectedType&& type ) = delete;
	
	
public:
	ReflectedType( const ReflectedType& type );
	ReflectedType& operator=( const ReflectedType& type );
};