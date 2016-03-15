#pragma once

// SIMD includes
#include "MathEngineDefs.h"

#include "Enum.h"
#include "Vect.h"

class Quat;



class Matrix
{
public:
	Matrix();
	Matrix( const Matrix& mtx );
	Matrix( const Vect& v0, const Vect& v1, const Vect& v2, const Vect& v3 );
	Matrix& operator=( const Matrix& mtx );
	~Matrix();

	Matrix( MatrixSpecialType type );
	Matrix( MatrixTransType, float x, float y, float z );
	Matrix( MatrixTransType, const Vect& trans );
	Matrix( MatrixScaleType, float sx, float sy, float sz );
	Matrix( MatrixScaleType, const Vect& scale );
	Matrix( RotType type, float angle );
	Matrix( RotOrientType type, const Vect& direction, const Vect& up );
	Matrix( RotAxisAngleType, const Vect& axis, float angle );
	Matrix( Rot3AxisType, float xAngle, float yAngle, float zAngle );
	Matrix( const Quat& quat );

	bool isEqual( const Matrix& mtx, float tolerance = MATH_TOLERANCE ) const;
	bool isIdentity( float tolerance = MATH_TOLERANCE ) const;

	void set( MatrixSpecialType type );
	void set( MatrixTransType, float x, float y, float z );
	void set( MatrixTransType, const Vect& v );
	void set( MatrixScaleType, float sx, float sy, float sz );
	void set( MatrixScaleType, const Vect& scale );
	void set( RotType type, float angle );
	void set( RotOrientType type, const Vect& direction, const Vect& up );
	void set( Rot3AxisType, float xAngle, float yAngle, float zAngle );
	void set( RotAxisAngleType, const Vect& axis, float angle );
	void set( MatrixRowType row, const Vect& v );
	void set( const Vect& v0, const Vect& v1, const Vect& v2, const Vect& v3 );
	void set( const Quat& q );

	Vect get( MatrixRowType row ) const;

	inline float operator[]( m0_enum ) const { return v0[X]; }
	inline float operator[]( m1_enum ) const { return v0[Y]; }
	inline float operator[]( m2_enum ) const { return v0[Z]; }
	inline float operator[]( m3_enum ) const { return v0[W]; }
	inline float operator[]( m4_enum ) const { return v1[X]; }
	inline float operator[]( m5_enum ) const { return v1[Y]; }
	inline float operator[]( m6_enum ) const { return v1[Z]; }
	inline float operator[]( m7_enum ) const { return v1[W]; }
	inline float operator[]( m8_enum ) const { return v2[X]; }
	inline float operator[]( m9_enum ) const { return v2[Y]; }
	inline float operator[]( m10_enum ) const { return v2[Z]; }
	inline float operator[]( m11_enum ) const { return v2[W]; }
	inline float operator[]( m12_enum ) const { return v3[X]; }
	inline float operator[]( m13_enum ) const { return v3[Y]; }
	inline float operator[]( m14_enum ) const { return v3[Z]; }
	inline float operator[]( m15_enum ) const { return v3[W]; }

	inline float& operator[]( m0_enum ) { return v0[X]; }
	inline float& operator[]( m1_enum ) { return v0[Y]; }
	inline float& operator[]( m2_enum ) { return v0[Z]; }
	inline float& operator[]( m3_enum ) { return v0[W]; }
	inline float& operator[]( m4_enum ) { return v1[X]; }
	inline float& operator[]( m5_enum ) { return v1[Y]; }
	inline float& operator[]( m6_enum ) { return v1[Z]; }
	inline float& operator[]( m7_enum ) { return v1[W]; }
	inline float& operator[]( m8_enum ) { return v2[X]; }
	inline float& operator[]( m9_enum ) { return v2[Y]; }
	inline float& operator[]( m10_enum ) { return v2[Z]; }
	inline float& operator[]( m11_enum ) { return v2[W]; }
	inline float& operator[]( m12_enum ) { return v3[X]; }
	inline float& operator[]( m13_enum ) { return v3[Y]; }
	inline float& operator[]( m14_enum ) { return v3[Z]; }
	inline float& operator[]( m15_enum ) { return v3[W]; }

	// Add or subtract two matrices
	Matrix operator+( const Matrix& mtx ) const;
	Matrix operator-( const Matrix& mtx ) const;

	// Add or subtract two matrices in place
	void operator+=( const Matrix& mtx );
	void operator-=( const Matrix& mtx );

	// Multiply the matrix by a scalar
	Matrix operator*( float a ) const;
	void operator*=( float a );

	// Multiply two matrices
	Matrix operator*( const Matrix& mtx ) const;
	Matrix& operator*=( const Matrix& mtx );

	// Does nothing
	Matrix operator+( ) const;

	// Negates all elements in the matrix
	Matrix operator-( ) const;

	// Returns the determinant of the matrix
	float det() const;

	// Returns the transpose of the matrix
	Matrix& T();
	Matrix getT() const;

	// Returns the inverse of the matrix
	Matrix& inv();
	Matrix getInv() const;

private:

	Matrix( const M128_TYPE& row0, const M128_TYPE& row1, const M128_TYPE& row2, const M128_TYPE& row3 );
	friend class Vect;
	friend class Quat;

	Vect v0;
	Vect v1;
	Vect v2;
	Vect v3;
};

Matrix operator*( float a, const Matrix& mtx );