#pragma once

class Readable
{
protected:
	Readable() { }
	Readable( const Readable& ) { }
	Readable( Readable&& ) { }
	Readable& operator=( const Readable& ) { return *this; }
	Readable& operator=( Readable&& ) { return *this; }

public:
	virtual ~Readable() { }

	virtual char* Read_Bytes( int bytesToRead ) abstract;
	virtual char* Read_Token() abstract;
};