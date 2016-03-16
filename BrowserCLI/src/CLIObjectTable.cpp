#include <vcclr.h>
#include <GameAssert.h>

#include "CLIObjectTable.h"

namespace CLI
{

	struct CLIObjectValue
	{
		const void* ptr;
		CLIObjectValue* next;
		gcroot<Object^> obj;
	};

	CLIObjectTable::CLIObjectTable( unsigned int inTableSize )
		: table( new CLIObjectValue*[inTableSize] ),
		tableSize( inTableSize )
	{
		GameAssert( inTableSize > 0 );
		memset( table, 0, sizeof( CLIObjectValue* ) * inTableSize );
	}

	CLIObjectTable::~CLIObjectTable()
	{
		for( unsigned int i = 0; i < tableSize; i++ )
		{
			CLIObjectValue* curr = table[i];
			while( curr != nullptr )
			{
				// Got to tell the garbage collector that we aren't referencing the obj anymore
				curr->obj = nullptr;
				CLIObjectValue* next = curr->next;
				delete curr;
				curr = next;
			}
		}

		delete table;
	}

	void CLIObjectTable::Add( const void* ptr, Object^ obj )
	{
		GameAssert( ptr != nullptr );
		GameAssert( obj != nullptr );

		// Most addresses are 4-byte aligned, so we don't want to have a crap ton of collisions in a couple of indices
		unsigned int index = ( (unsigned int) ptr / 4 ) % tableSize;

		CLIObjectValue* entry = new CLIObjectValue();
		entry->ptr = ptr;
		entry->next = table[index];
		entry->obj = obj;

		table[index] = entry;
	}

	Object^ CLIObjectTable::operator[] ( const void* ptr )
	{
		GameAssert( ptr != nullptr );

		// Most addresses are 4-byte aligned, so we don't want to have a crap ton of collisions in a couple of indices
		unsigned int index = ( (unsigned int) ptr / 4 ) % tableSize;

		CLIObjectValue* entry = table[index];
		while( entry != nullptr && entry->ptr != ptr )
		{
			entry = entry->next;
		}

		if( entry == nullptr )
		{
			return nullptr;
		}
		else
		{
			return entry->obj;
		}
	}

	Object^ CLIObjectTable::Remove( const void* ptr )
	{
		GameAssert( ptr != nullptr );

		// Most addresses are 4-byte aligned, so we don't want to have a crap ton of collisions in a couple of indices
		unsigned int index = ( (unsigned int) ptr / 4 ) % tableSize;

		CLIObjectValue* entry = table[index];
		CLIObjectValue* prev = nullptr;

		while( entry != nullptr && entry->ptr != ptr )
		{
			prev = entry;
			entry = entry->next;
		}

		if( entry == nullptr )
		{
			return nullptr;
		}

		Object^ obj = entry->obj;

		if( prev == nullptr )
		{
			table[index] = entry->next;
		}
		else
		{
			prev->next = entry->next;
		}

		// Gotta tell the garbage collector that we aren't referencing the object any more
		entry->obj = nullptr;
		delete entry;

		return obj;
	}
}