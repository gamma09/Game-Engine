#pragma once

class Triangle
{
public:
	Triangle( const unsigned int index0, const unsigned int index1, const unsigned int index2 );
	Triangle( const Triangle& t );
	Triangle& operator=( const Triangle& t );
	~Triangle();


private:
	Triangle();

public:

	// corresponds to indices in the vertexList in ModelData class
	unsigned int vert_index[3];
};