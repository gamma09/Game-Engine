#pragma once

class BaseClass
{
protected:
	BaseClass( const char* name );

public:
	virtual ~BaseClass();
	
	// TODO with editor only
	inline const char* GetName() const { return this->name; }
	
private:
	char* name;
	
	
private:
	BaseClass( const BaseClass& type ) = delete;
	BaseClass( BaseClass&& type ) = delete;
	BaseClass& operator=( const BaseClass& type ) = delete;
	BaseClass& operator=( BaseClass&& type ) = delete;
};