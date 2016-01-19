#pragma once

#include "Component.h"

class TextField : public Component
{
public:
	TextField();
	TextField( const Rectangle& rect );
	TextField( const char* initialValue );
	TextField( const Rectangle& rect, const char* initialValue );
	virtual ~TextField();

	void Set_Value( const char* value ) const;
	const char* Get_Value() const;
};