#pragma once

class Vertex
{
public:
	Vertex();
	Vertex( const Vertex& v );
	Vertex& operator=( const Vertex& v );
	~Vertex();


	bool Equals( const Vertex& v, const float tolerance = 0.002f ) const;



public:

	// position
	float x;
	float y;
	float z;

	// texture coordinates
	float u;
	float v;

	// surface normal
	float nx;
	float ny;
	float nz;
};