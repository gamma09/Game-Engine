#include <GameAssert.h>
#include <StringUtils.h>

#include "ManagedObject.h"

ManagedObject::ManagedObject() :
	next(0),
	prev(0),
	isActive(false)
{
	// Do nothing
}

ManagedObject::ManagedObject(const ManagedObject& obj) :
	next(obj.next),
	prev(obj.prev),
	isActive(obj.isActive)
{
	// Do nothing
}

ManagedObject& ManagedObject::operator=(const ManagedObject& obj)
{
	this->next = obj.next;
	this->prev = obj.prev;
	this->isActive = obj.isActive;

	return *this;
}

ManagedObject::~ManagedObject()
{
	// Do nothing
}

void ManagedObject::SetID( const char* id )
{
#if defined(DEBUG) || defined(_DEBUG)
	copy_string( this->ID, id );
#else
	id;
#endif
}

void ManagedObject::Reset()
{
	GameAssert(this->isActive);

	this->isActive = false;
	this->next = 0;
	this->prev = 0;
}