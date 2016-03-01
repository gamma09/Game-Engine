#pragma once

#include <GameAssert.h>
#include "BaseClass.h"

template< typename T >
extern void SerializeClass( T* t, Writeable& writer );

template< typename T >
extern void DeserializeClass( T* t, Readable& reader );

template< typename T >
class Class : public BaseClass
{
public:

	virtual void Serialize( Object* instance, Writeable& writer ) override
	{
		T* myInstance = reinterpret_cast<T*>( instance );
		GameAssert( myInstance );

		SerializeClass<T>( myInstance, writer );
	}

	virtual void Deserialize( Object* instance, Readable& reader ) override
	{
		T* myInstance = reinterpret_cast<T*>( instance );
		GameAssert( myInstance );

		DeserializeClass<T>( myInstance, reader );
	}

	static const Class<T>& Load();

private:
	Class( const char* name, const BaseClass* parent ) : BaseClass( name, parent, sizeof( T ) ) { }
	virtual ~Class() { }
	
	Class( Class&& type ) = delete;
	Class& operator=( Class&& type ) = delete;
	Class( const Class& type ) = delete;
	Class& operator=( const Class& type ) = delete;

	friend class ClassLoader;
};
