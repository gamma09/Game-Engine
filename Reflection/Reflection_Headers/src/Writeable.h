#pragma once

class Writeable
{
protected:
	Writeable() { }
	Writeable( const Writeable& ) { }
	Writeable( Writeable&& ) { }
	Writeable& operator=( const Writeable& ) { return *this; }
	Writeable& operator=( Writeable&& ) { return *this; }
	
public:
	virtual ~Writeable() { }

	virtual void Write( const char* str ) abstract;
};