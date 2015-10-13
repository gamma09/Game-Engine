#include "Moveable.h"

void Moveable::Add_Velocity(const Vect& _v)
{
	this->v += _v;
}

void Moveable::Add_Velocity(const float vx, const float vy, const float vz)
{
	Vect _v(vx, vy, vz);
	this->v += _v;
}

const Vect& Moveable::Get_Velocity() const
{
	return this->v;
}

Moveable::Moveable() :
	v(),
	lastMovementTime(0)
{
	// Do nothing
}

Moveable::Moveable(const Moveable& moveable) :
	v(moveable.v)
{
	// Do nothing
}

Moveable& Moveable::operator=(const Moveable& moveable)
{
	this->v = moveable.v;
	return *this;
}

Moveable::~Moveable()
{
	// Do nothing
}