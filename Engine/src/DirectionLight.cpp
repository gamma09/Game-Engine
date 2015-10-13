#include <assert.h>

#include "DirectionLightManager.h"
#include "DirectionLight.h"

DirectionLight::DirectionLight() :
	ManagedObject(),
	ReferencedObject()
{
	// Do nothing
}

DirectionLight::~DirectionLight()
{
	// Do nothing
}

void DirectionLight::Set(float x, float y, float z, float r, float g, float b, float a)
{
	this->position.set(x, y, z);
	this->color.set(r, g, b, a);
}

void DirectionLight::Reset()
{
	assert(this->Get_Reference_Count() == 0);

	ManagedObject::Reset();
}

const Vect& DirectionLight::Get_Position() const
{
	return this->position;
}

const Vect& DirectionLight::Get_Color() const
{
	return this->color;
}

void DirectionLight::Free_Me()
{
	DirectionLightManager::Instance()->Remove(this);
}