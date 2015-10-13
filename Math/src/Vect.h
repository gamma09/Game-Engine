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
	Vect(const Vect& v);

	Vect(const Vect& v, const float w);

	// Specialization ctor, w is optional (defaults to 1 if not specified)
	Vect(const float x, const float y, const float z, const float w = 1.0f);

	// Assignment operator
	Vect& operator=(const Vect& v);

	// Destructor
	~Vect();

	// Checks for element equality within a specified tolerance
	// Tolerance is optional
	const bool isEqual(const Vect& rhs, const float tolerance = MATH_TOLERANCE) const;

	// Checks whether the vector contains all zeros within a specified tolerance
	// Tolerance is optional
	const bool isZero(const float tolerance = MATH_TOLERANCE) const;

	// Sets the vector to the specified values - w is optional and defaults to 1
	void set(const float x, const float y, const float z, const float w = 1.0f);
	void set(const Vect& v);

	// get operators for x, y, z, and w
	const float operator[] (const x_enum) const;
	const float operator[] (const y_enum) const;
	const float operator[] (const z_enum) const;
	const float operator[] (const w_enum) const;

	// set operators for x, y, z, and w
	float& operator[] (const x_enum);
	float& operator[] (const y_enum);
	float& operator[] (const z_enum);
	float& operator[] (const w_enum);

	// Binary add and subtract operators
	const Vect operator+(const Vect& rhs) const;
	const Vect operator-(const Vect& rhs) const;

	// In-place add and subtract operators
	void operator+=(const Vect& rhs);
	void operator-=(const Vect& rhs);

	// Dot product of two vectors
	const float dot(const Vect& v2) const;

	// Cross product of two vectors
	const Vect Vect::cross(const Vect& v2) const;

	// Normalize the vector in place
	void norm();

	// Get a normalized vector without modifying the original
	const Vect getNorm() const;

	// Gets the magnitude of the vector
	const float mag() const;

	// Gets the square of the magnitude of the vector
	const float magSqr() const;

	// Returns the angle between this vector and v2
	const float getAngle(const Vect& v2) const;

	// Scaling operators
	const Vect operator*(const float a) const;
	void operator*=(const float a);

	// Unary + operator, does nothing
	const Vect operator+() const;
	// Unary - operator, negates vector
	const Vect operator-() const;

	// multiply a vector by a matrix
	const Vect operator*(const Matrix& mtx) const;
	Vect& operator*=(const Matrix& mtx);

	friend class Matrix;
	friend class Quat;
	friend class VectApp;

private:

	Vect(const M128_TYPE& m);
	Vect(const M128_TYPE& m, const float w);
	Vect(const Vect& v, const NormType&);
	

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

const Vect operator*(const float a, const Vect& b);