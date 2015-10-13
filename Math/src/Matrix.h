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
	Matrix(const Matrix& mtx);
	Matrix(const Vect& v0, const Vect& v1, const Vect& v2, const Vect& v3);
	Matrix& operator=(const Matrix& mtx);
	~Matrix();

	Matrix(const MatrixSpecialType type);
	Matrix(const MatrixTransType, const float x, const float y, const float z);
	Matrix(const MatrixTransType, const Vect& trans);
	Matrix(const MatrixScaleType, const float sx, const float sy, const float sz);
	Matrix(const MatrixScaleType, const Vect& scale);
	Matrix(const RotType type, const float angle);
	Matrix(const RotOrientType type, const Vect& direction, const Vect& up);
	Matrix(const RotAxisAngleType, const Vect& axis, const float& angle);
	Matrix(const Rot3AxisType, const float& xAngle, const float& yAngle, const float& zAngle);
	Matrix(const Quat& quat);

	const bool isEqual(const Matrix& mtx, const float tolerance = MATH_TOLERANCE) const;
	const bool isIdentity(const float tolerance = MATH_TOLERANCE) const;

	void set(const MatrixSpecialType type);
	void set(const MatrixTransType, const float x, const float y, const float z);
	void set(const MatrixTransType, const Vect& v);
	void set(const MatrixScaleType, const float sx, const float sy, const float sz);
	void set(const MatrixScaleType, const Vect& scale);
	void set(const RotType type, const float angle);
	void set(const RotOrientType type, const Vect& direction, const Vect& up);
	void set(const Rot3AxisType, const float& xAngle, const float& yAngle, const float& zAngle);
	void set(const RotAxisAngleType, const Vect& axis, const float& angle);
	void set(const MatrixRowType row, const Vect& v);
	void set(const Vect& v0, const Vect& v1, const Vect& v2, const Vect& v3);
	void set(const Quat& q);

	const Vect get(const MatrixRowType row) const;

	const float operator[](const m0_enum) const;
	const float operator[](const m1_enum) const;
	const float operator[](const m2_enum) const;
	const float operator[](const m3_enum) const;
	const float operator[](const m4_enum) const;
	const float operator[](const m5_enum) const;
	const float operator[](const m6_enum) const;
	const float operator[](const m7_enum) const;
	const float operator[](const m8_enum) const;
	const float operator[](const m9_enum) const;
	const float operator[](const m10_enum) const;
	const float operator[](const m11_enum) const;
	const float operator[](const m12_enum) const;
	const float operator[](const m13_enum) const;
	const float operator[](const m14_enum) const;
	const float operator[](const m15_enum) const;

	float& operator[](const m0_enum);
	float& operator[](const m1_enum);
	float& operator[](const m2_enum);
	float& operator[](const m3_enum);
	float& operator[](const m4_enum);
	float& operator[](const m5_enum);
	float& operator[](const m6_enum);
	float& operator[](const m7_enum);
	float& operator[](const m8_enum);
	float& operator[](const m9_enum);
	float& operator[](const m10_enum);
	float& operator[](const m11_enum);
	float& operator[](const m12_enum);
	float& operator[](const m13_enum);
	float& operator[](const m14_enum);
	float& operator[](const m15_enum);

	// Add or subtract two matrices
	const Matrix operator+(const Matrix& mtx) const;
	const Matrix operator-(const Matrix& mtx) const;

	// Add or subtract two matrices in place
	void operator+=(const Matrix& mtx);
	void operator-=(const Matrix& mtx);

	// Multiply the matrix by a scalar
	const Matrix operator*(const float a) const;
	void operator*=(const float a);

	// Multiply two matrices
	const Matrix operator*(const Matrix& mtx) const;
	Matrix& operator*=(const Matrix& mtx);

	// Does nothing
	const Matrix operator+() const;

	// Negates all elements in the matrix
	const Matrix operator-() const;

	// Returns the determinant of the matrix
	const float det() const;

	// Returns the transpose of the matrix
	Matrix& T();
	const Matrix getT() const;

	// Returns the inverse of the matrix
	Matrix& inv();
	const Matrix getInv() const;

private:

	Matrix(const M128_TYPE& row0, const M128_TYPE& row1, const M128_TYPE& row2, const M128_TYPE& row3);
	friend class Vect;
	friend class Quat;

// Level 4 complains nameless struct/union ...
#pragma warning( disable : 4201)

	union 
	{
		struct 
		{
			Vect v0;
			Vect v1;
			Vect v2;
			Vect v3;
		};

		struct 
		{
			float m0;
			float m1;
			float m2;
			float m3;
			float m4;
			float m5;
			float m6;	
			float m7;
			float m8;
			float m9;
			float m10;
			float m11;
			float m12;
			float m13;
			float m14;
			float m15;
		};
	};
};

const Matrix operator*(const float a, const Matrix& mtx);