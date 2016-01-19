#pragma once

class Rectangle
{
public:
	Rectangle();
	Rectangle( int x, int y, int width, int height );
	Rectangle( const Rectangle& rect );
	Rectangle& operator=( const Rectangle& rect );
	~Rectangle();


	int x;
	int y;
	int width;
	int height;
};