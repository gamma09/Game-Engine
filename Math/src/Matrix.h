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

	Matrix( const MatrixSpecialType type );
	Matrix( const MatrixTransType, const float x, const float y, const float z );
	Matrix( const MatrixTransType, const Vect& trans );
	Matrix( const MatrixScaleType, const float sx, const float sy, const float sz );
	Matrix( const MatrixScaleType, const Vect& scale );
	Matrix( const RotType type, const float angle );
	Matrix( const RotOrientType type, const Vect& direction, const Vect& up );
	Matrix( const RotAxisAngleType, const Vect& axis, const float& angle );
	Matrix( const Rot3AxisType, const float& xAngle, const float& yAngle, const float& zAngle );
	Matrix( const Quat& quat );

	const bool isEqual( const Matrix& mtx, const float tolerance = MATH_TOLERANCE ) const;
	const bool isIdentity( const float tolerance = MATH_TOLERANCE ) const;

	void set( const MatrixSpecialType type );
	void set( const MatrixTransType, const float x, const float y, const float z );
	void set( const MatrixTransType, const Vect& v );
	void set( const MatrixScaleType, const float sx, const float sy, const float sz );
	void set( const MatrixScaleType, const Vect& scale );
	void set( const RotType type, const float angle );
	void set( const RotOrientType type, const Vect& direction, const Vect& up );
	void set( const Rot3AxisType, const float& xAngle, const float& yAngle, const float& zAngle );
	void set( const RotAxisAngleType, const Vect& axis, const float& angle );
	void set( const MatrixRowType row, const Vect& v );
	void set( const Vect& v0, const Vect& v1, const Vect& v2, const Vect& v3 );
	void set( const Quat& q );

	const Vect get( const MatrixRowType row ) const;

	inline const float operator[]( const m0_enum ) const { return v0[X]; }
	inline const float operator[]( const m1_enum ) const { return v0[Y]; }
	inline const float operator[]( const m2_enum ) const { return v0[Z]; }
	inline const float operator[]( const m3_enum ) const { return v0[W]; }
	inline const float operator[]( const m4_enum ) const { return v1[X]; }
	inline const float operator[]( const m5_enum ) const { return v1[Y]; }
	inline const float operator[]( const m6_enum ) const { return v1[Z]; }
	inline const float operator[]( const m7_enum ) const { return v1[W]; }
	inline const float operator[]( const m8_enum ) const { return v2[X]; }
	inline const float operator[]( const m9_enum ) const { return v2[Y]; }
	inline const float operator[]( const m10_enum ) const { return v2[Z]; }
	inline const float operator[]( const m11_enum ) const { return v2[W]; }
	inline const float operator[]( const m12_enum ) const { return v3[X]; }
	inline const float operator[]( const m13_enum ) const { return v3[Y]; }
	inline const float operator[]( const m14_enum ) const { return v3[Z]; }
	inline const float operator[]( const m15_enum ) const { return v3[W]; }

	inline float& operator[]( const m0_enum ) { return v0[X]; }
	inline float& operator[]( const m1_enum ) { return v0[Y]; }
	inline float& operator[]( const m2_enum ) { return v0[Z]; }
	inline float& operator[]( const m3_enum ) { return v0[W]; }
	inline float& operator[]( const m4_enum ) { return v1[X]; }
	inline float& operator[]( const m5_enum ) { return v1[Y]; }
	inline float& operator[]( const m6_enum ) { return v1[Z]; }
	inline float& operator[]( const m7_enum ) { return v1[W]; }
	inline float& operator[]( const m8_enum ) { return v2[X]; }
	inline float& operator[]( const m9_enum ) { return v2[Y]; }
	inline float& operator[]( const m10_enum ) { return v2[Z]; }
	inline float& operator[]( const m11_enum ) { return v2[W]; }
	inline float& operator[]( const m12_enum ) { return v3[X]; }
	inline float& operator[]( const m13_enum ) { return v3[Y]; }
	inline float& operator[]( const m14_enum ) { return v3[Z]; }
	inline float& operator[]( const m15_enum ) { return v3[W]; }

	// Add or subtract two matrices
	const Matrix operator+( const Matrix& mtx ) const;
	const Matrix operator-( const Matrix& mtx ) const;

	// Add or subtract two matrices in place
	void operator+=( const Matrix& mtx );
	void operator-=( const Matrix& mtx );

	// Multiply the matrix by a scalar
	const Matrix operator*( const float a ) const;
	void operator*=( const float a );

	// Multiply two matrices
	const Matrix operator*( const Matrix& mtx ) const;
	Matrix& operator*=( const Matrix& mtx );

	// Does nothing
	const Matrix operator+( ) const;

	// Negates all elements in the matrix
	const Matrix operator-( ) const;

	// Returns the determinant of the matrix
	const float det() const;

	// Returns the transpose of the matrix
	Matrix& T();
	const Matrix getT() const;

	// Returns the inverse of the matrix
	Matrix& inv();
	const Matrix getInv() const;

private:

	Matrix( const M128_TYPE& row0, const M128_TYPE& row1, const M128_TYPE& row2, const M128_TYPE& row3 );
	friend class Vect;
	friend class Quat;

	Vect v0;
	Vect v1;
	Vect v2;
	Vect v3;
};

const Matrix operator*( const float a, const Matrix& mtx );