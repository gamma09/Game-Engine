#include "Vertex.h"

#include <cmath>

Vertex::Vertex()
{
	// Do nothing
}

Vertex::Vertex( const Vertex& v ) :
	x( v.x ),
	y( v.y ),
	z( v.z ),
	u( v.u ),
	v( v.v ),
	nx( v.nx ),
	ny( v.ny ),
	nz( v.nz )
{
	// Do nothing
}

Vertex& Vertex::operator=( const Vertex& v )
{
	this->x = v.x;
	this->y = v.y;
	this->z = v.z;
	this->u = v.u;
	this->v = v.v;
	this->nx = v.nx;
	this->ny = v.ny;
	this->nz = v.nz;

	return *this;
}

Vertex::~Vertex()
{
	// Do nothing
}

bool Vertex::Equals( const Vertex& v, const float tolerance ) const
{
	return fabsf( this->x - v.x ) < tolerance &&
		fabsf( this->y - v.y ) < tolerance &&
		fabsf( this->z - v.z ) < tolerance &&
		fabsf( this->u - v.u ) < tolerance &&
		fabsf( this->v - v.v ) < tolerance &&
		fabsf( this->nx - v.nx ) < tolerance &&
		fabsf( this->ny - v.ny ) < tolerance &&
		fabsf( this->nz - v.nz ) < tolerance;
}