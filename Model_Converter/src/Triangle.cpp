#include "Triangle.h"

Triangle::Triangle( const unsigned int index0, const unsigned int index1, const unsigned int index2 )
{
	this->vert_index[0] = index0;
	this->vert_index[1] = index1;
	this->vert_index[2] = index2;
}

Triangle::Triangle( const Triangle& t )
{
	this->vert_index[0] = t.vert_index[0];
	this->vert_index[1] = t.vert_index[1];
	this->vert_index[2] = t.vert_index[2];
}

Triangle& Triangle::operator=( const Triangle& t )
{
	this->vert_index[0] = t.vert_index[0];
	this->vert_index[1] = t.vert_index[1];
	this->vert_index[2] = t.vert_index[2];

	return *this;
}

Triangle::~Triangle()
{
	// Do nothing
}