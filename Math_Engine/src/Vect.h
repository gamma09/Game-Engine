#pragma once

// Includes to handle SIMD register types
#include "MathEngineDefs.h"
#include "Constants.h"
#include "Enum.h"
// forward declare
class Matrix;

enum NormType
{
	NORMALIZE
};

class Vect
{
public:

	// Creates a vector with the values <0, 0, 0, 1>
	Vect();

	// Copies a vector
	Vect( const Vect& v );

	Vect( const Vect& v, float w );

	// Specialization ctor, w is optional (defaults to 1 if not specified)
	Vect( float x, float y, float z, float w = 1.0f );

	// Assignment operator
	Vect& operator=( const Vect& v );

	// Destructor
	~Vect();

	// Checks for element equality within a specified tolerance
	// Tolerance is optional
	bool isEqual( const Vect& rhs, float tolerance = MATH_TOLERANCE ) const;

	// Checks whether the vector contains all zeros within a specified tolerance
	// Tolerance is optional
	bool isZero( float tolerance = MATH_TOLERANCE ) const;

	// Sets the vector to the specified values - w is optional and defaults to 1
	void set( float x, float y, float z, float w = 1.0f );
	void set( const Vect& v );

	// get operators for x, y, z, and w
	inline float operator[] ( const x_enum ) const { return this->x; }
	inline float operator[] ( const y_enum ) const { return this->y; }
	inline float operator[] ( const z_enum ) const { return this->z; }
	inline float operator[] ( const w_enum ) const { return this->w; }

	// set operators for x, y, z, and w
	inline float& operator[] ( const x_enum ) { return this->x; }
	inline float& operator[] ( const y_enum ) { return this->y; }
	inline float& operator[] ( const z_enum ) { return this->z; }
	inline float& operator[] ( const w_enum ) { return this->w; }

	// Binary add and subtract operators
	Vect operator+( const Vect& rhs ) const;
	Vect operator-( const Vect& rhs ) const;

	// In-place add and subtract operators
	void operator+=( const Vect& rhs );
	void operator-=( const Vect& rhs );

	// Dot product of two vectors
	float dot( const Vect& v2 ) const;

	// Cross product of two vectors
	Vect Vect::cross( const Vect& v2 ) const;

	// Normalize the vector in place
	void norm();

	// Get a normalized vector without modifying the original
	Vect getNorm() const;

	// Gets the magnitude of the vector
	float mag() const;

	// Gets the square of the magnitude of the vector
	float magSqr() const;

	// Returns the angle between this vector and v2
	float getAngle( const Vect& v2 ) const;

	// Scaling operators
	Vect operator*( float a ) const;
	void operator*=( float a );

	// Unary + operator, does nothing
	Vect operator+( ) const;
	// Unary - operator, negates vector
	Vect operator-( ) const;

	// multiply a vector by a matrix
	Vect operator*( const Matrix& mtx ) const;
	Vect& operator*=( const Matrix& mtx );

	friend class Matrix;
	friend class Quat;
	friend class VectApp;

private:

	Vect( const M128_TYPE& m );
	Vect( const M128_TYPE& m, float w );
	Vect( const Vect& v, const NormType& );


public:
	// Level 4 complains nameless struct/union ...
#pragma warning( disable : 4201)
	// anonymous union
	union
	{
		struct
		{
			M128_TYPE m;
		};

		// anonymous struct
		struct
		{
			float x;
			float y;
			float z;
			float w;
		};
	};


};

Vect operator*( float a, const Vect& b );