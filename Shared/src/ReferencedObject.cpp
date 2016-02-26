#include "GameAssert.h"

#include "ReferencedObject.h"

ReferencedObject::ReferencedObject() :
	reference_count(0)
{
	// Do nothing
}

ReferencedObject::~ReferencedObject()
{
	// Do nothing
}

void ReferencedObject::Add_Reference()
{
	this->reference_count++;
}

void ReferencedObject::Remove_Reference()
{
	GameAssert(this->reference_count > 0);

	this->reference_count--;
	if (this->reference_count == 0)
		this->Free_Me();
}

const unsigned int ReferencedObject::Get_Reference_Count() const
{
	return this->reference_count;
}