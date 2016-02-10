#pragma once

#include <string.h>
#include <GameAssert.h>
#include <BaseClass.h>

class Variable
{
private:
	Variable( const char* inName, const BaseClass& inType, unsigned int inOffset );
	
public:
	
	Variable( const Variable& var );
	Variable( Variable&& var );
	~Variable();
	
	inline const char* GetName() const { return this->name; }
	inline const BaseClass& GetType() const { return this->type; }
	inline unsigned int GetOffset() const { return this->offset; }
	
	
	
private:
	Variable& operator=( const Variable& var ) = delete;
	Variable& operator=( Variable&& var ) = delete;
	
	
private:
	const BaseClass& type;
	unsigned int offset;
	
	// TODO with editor
	char* name;
	
	
	friend class ClassLoader;
};