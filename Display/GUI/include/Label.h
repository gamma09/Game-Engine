#pragma once

#include "Component.h"

class Label : public Component
{
public:
	Label();
	Label( const Rectangle& rect );
	Label( const char* text );
	Label( const Rectangle& rect, const char* text );
	virtual ~Label();
};