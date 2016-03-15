#pragma once

#include <Vect.h>

class Moveable abstract
{
public:

	void Add_Velocity( const Vect& v );
	void Add_Velocity( float vx, float vy, float vz );
	const Vect& Get_Velocity() const;

	virtual void Update_Position( uint32_t time ) abstract;

protected:
	Moveable();
	Moveable( const Moveable& moveable );
	Moveable& operator=( const Moveable& moveable );
	virtual ~Moveable();



	uint32_t lastMovementTime;

private:
	Vect v;

};