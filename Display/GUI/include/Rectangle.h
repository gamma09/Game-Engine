#pragma once

namespace GUI
{
	class Rectangle
	{
	public:
		Rectangle();
		Rectangle( int x, int y, int width, int height );
		Rectangle( const GUI::Rectangle& rect );
		Rectangle& operator=( const GUI::Rectangle& rect );
		~Rectangle();


		int x;
		int y;
		int width;
		int height;
	};
}