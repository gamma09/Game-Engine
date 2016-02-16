#pragma once

#include "Writeable.h"
#include "Readable.h"

// Do not include Object.h in this file! (or in BaseClass.cpp or in Class.h)
// There is some sort of compiling dependency that gets screwed if that happens.
class Object;
class Variable;

class BaseClass
{
protected:
	BaseClass( const char* name, const BaseClass* parent, unsigned int size );

public:
	virtual ~BaseClass();

	inline const char* GetName() const { return this->name; }
	inline const BaseClass* GetParent() const { return this->parent; }
	inline unsigned int GetClassSize() const { return this->size; }

	virtual void Serialize( Object* instance, Writeable& writer ) abstract;
	virtual void Deserialize( Object* instance, Readable& reader ) abstract;
	

private:
	const BaseClass* parent;
	unsigned int size;
	Variable* variableHead;
	char* name;
	
	
private:
	BaseClass( const BaseClass& type ) = delete;
	BaseClass( BaseClass&& type ) = delete;
	BaseClass& operator=( const BaseClass& type ) = delete;
	BaseClass& operator=( BaseClass&& type ) = delete;

	friend class ClassLoader;
};