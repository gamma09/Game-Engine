#include "Matrix.h"
#include <math.h>
#include <GameAssert.h>

#include "Quat.h"

const static Vect ZERO_ROW(0.0f, 0.0f, 0.0f, 0.0f);

const static Vect IDENTITY_ROW_0(1.0f, 0.0f, 0.0f, 0.0f);
const static Vect IDENTITY_ROW_1(0.0f, 1.0f, 0.0f, 0.0f);
const static Vect IDENTITY_ROW_2(0.0f, 0.0f, 1.0f, 0.0f);
const static Vect IDENTITY_ROW_3(0.0f, 0.0f, 0.0f, 1.0f);

Matrix::Matrix() :
	v0(ZERO_ROW),
	v1(ZERO_ROW),
	v2(ZERO_ROW),
	v3(ZERO_ROW)
{
	CHECK_ALIGNMENT_16(this);
}

Matrix::Matrix(const Matrix& mtx) :
	v0(mtx.v0),
	v1(mtx.v1),
	v2(mtx.v2),
	v3(mtx.v3)
{
	CHECK_ALIGNMENT_16(this);
}

Matrix::Matrix(const Vect& _v0, const Vect& _v1, const Vect& _v2, const Vect& _v3) :
	v0(_v0),
	v1(_v1),
	v2(_v2),
	v3(_v3)
{
	CHECK_ALIGNMENT_16(this);
}

Matrix::Matrix(const M128_TYPE& row0, const M128_TYPE& row1, const M128_TYPE& row2, const M128_TYPE& row3) :
	v0(row0),
	v1(row1),
	v2(row2),
	v3(row3)
{
	CHECK_ALIGNMENT_16(this);
}

Matrix::Matrix(const MatrixSpecialType type)
{
	CHECK_ALIGNMENT_16(this);

	this->set(type);
}

Matrix::Matrix(const MatrixTransType, const float x, const float y, const float z) :
	v0(IDENTITY_ROW_0),
	v1(IDENTITY_ROW_1),
	v2(IDENTITY_ROW_2),
	v3(x, y, z)
{
	CHECK_ALIGNMENT_16(this);
}

Matrix::Matrix(const MatrixTransType, const Vect& trans) :
	v0(IDENTITY_ROW_0),
	v1(IDENTITY_ROW_1),
	v2(IDENTITY_ROW_2),
	v3(trans)
{
	CHECK_ALIGNMENT_16(this);
}

Matrix::Matrix(const MatrixScaleType, const float sx, const float sy, const float sz)
{
	CHECK_ALIGNMENT_16(this);

	this->set(SCALE, sx, sy, sz);
}

Matrix::Matrix(const MatrixScaleType, const Vect& scale)
{
	CHECK_ALIGNMENT_16(this);

	this->set(SCALE, scale);
}

Matrix::Matrix(const RotOrientType type, const Vect& direction, const Vect& up) :
	v3()
{
	CHECK_ALIGNMENT_16(this);

	this->set(type, direction, up);
}

Matrix::Matrix(const RotAxisAngleType, const Vect& axis, const float& angle)
{
	CHECK_ALIGNMENT_16(this);

	this->set(ROT_AXIS_ANGLE, axis, angle);
}

Matrix::Matrix(const Rot3AxisType, const float& xAngle, const float& yAngle, const float& zAngle)
{
	CHECK_ALIGNMENT_16(this);

	this->set(ROT_XYZ, xAngle, yAngle, zAngle);
}

Matrix::Matrix(const Quat& quat)
{
	CHECK_ALIGNMENT_16(this);

	this->set(quat);
}

Matrix::Matrix(const RotType type, const float angle)
{
	CHECK_ALIGNMENT_16(this);

	this->set(type, angle);
}

Matrix& Matrix::operator=(const Matrix& mtx)
{
	this->v0 = mtx.v0;
	this->v1 = mtx.v1;
	this->v2 = mtx.v2;
	this->v3 = mtx.v3;

	return *this;
}

Matrix::~Matrix()
{
	// Do nothing
}

const bool Matrix::isEqual(const Matrix& mtx, const float tolerance) const
{
	return (this->v0.isEqual(mtx.v0, tolerance) &&
			this->v1.isEqual(mtx.v1, tolerance) &&
			this->v2.isEqual(mtx.v2, tolerance) &&
			this->v3.isEqual(mtx.v3, tolerance));
}

const bool Matrix::isIdentity(const float tolerance) const
{
	return (this->v0.isEqual(IDENTITY_ROW_0, tolerance) &&
			this->v1.isEqual(IDENTITY_ROW_1, tolerance) &&
			this->v2.isEqual(IDENTITY_ROW_2, tolerance) &&
			this->v3.isEqual(IDENTITY_ROW_3, tolerance));
}

void Matrix::set(const MatrixSpecialType type)
{
	switch (type)
	{
	case IDENTITY:
		this->v0 = IDENTITY_ROW_0;
		this->v1 = IDENTITY_ROW_1;
		this->v2 = IDENTITY_ROW_2;
		this->v3 = IDENTITY_ROW_3;
		break;

	case ZERO:
		this->v0 = ZERO_ROW;
		this->v1 = ZERO_ROW;
		this->v2 = ZERO_ROW;
		this->v3 = ZERO_ROW;
		break;

	default:
		GameAssert(0);
		break;
	}
}

void Matrix::set(const MatrixTransType, const float x, const float y, const float z)
{
	this->v0 = IDENTITY_ROW_0;
	this->v1 = IDENTITY_ROW_1;
	this->v2 = IDENTITY_ROW_2;

	this->m12 = x;
	this->m13 = y;
	this->m14 = z;
	this->m15 = 1.0f;
}

void Matrix::set(const MatrixTransType, const Vect& v)
{
	this->v0 = IDENTITY_ROW_0;
	this->v1 = IDENTITY_ROW_1;
	this->v2 = IDENTITY_ROW_2;
	this->v3 = v;
	this->m15 = 1.0f;
}


void Matrix::set(const MatrixScaleType, const float sx, const float sy, const float sz)
{
	this->v0 = ZERO_ROW;
	this->v1 = ZERO_ROW;
	this->v2 = ZERO_ROW;
	this->v3 = IDENTITY_ROW_3;

	this->m0 = sx;
	this->m5 = sy;
	this->m10 = sz;
}

void Matrix::set(const MatrixScaleType, const Vect& scale)
{
	this->v0 = ZERO_ROW;
	this->v1 = ZERO_ROW;
	this->v2 = ZERO_ROW;
	this->v3 = IDENTITY_ROW_3;

	this->m0 = scale.x;
	this->m5 = scale.y;
	this->m10 = scale.z;
}

void Matrix::set(const RotType type, const float angle)
{
	float cs = cosf(angle);
	float sn = sinf(angle);

	switch (type)
	{
	case ROT_X:
		this->v0 = IDENTITY_ROW_0;
		
		this->m4 = 0.0f;
		this->m5 = cs;
		this->m6 = sn;
		this->m7 = 0.0f;

		this->m8 = 0.0f;
		this->m9 = -sn;
		this->m10 = cs;
		this->m11 = 0.0f;
		break;

	case ROT_Y:
		this->m0 = cs;
		this->m1 = 0.0f;
		this->m2 = -sn;
		this->m3 = 0.0f;

		this->v1 = IDENTITY_ROW_1;

		this->m8 = sn;
		this->m9 = 0.0f;
		this->m10 = cs;
		this->m11 = 0.0f;
		break;

	case ROT_Z:
		this->m0 = cs;
		this->m1 = sn;
		this->m2 = 0.0f;
		this->m3 = 0.0f;

		this->m4 = -sn;
		this->m5 = cs;
		this->m6 = 0.0f;
		this->m7 = 0.0f;

		this->v2 = IDENTITY_ROW_2;
		break;

	default:
		GameAssert(0);
		break;
	}

	this->v3 = IDENTITY_ROW_3;
}

void Matrix::set(const RotOrientType type, const Vect& direction, const Vect& up)
{
	this->v2 = direction.getNorm();
	this->v0 = up.cross(this->v2);
	this->v0.norm();
	this->v1 = this->v2.cross(this->v0);
	this->v1.norm();

	this->m3 = 0.0f;
	this->m7 = 0.0f;
	this->m11 = 0.0f;

	this->v3 = IDENTITY_ROW_3;

	switch (type)
	{
	case ROT_ORIENT:
		// Do nothing
		break;

	case ROT_INVERSE_ORIENT:
		this->inv();
		break;

	default:
		GameAssert(0);
	}
}

void Matrix::set(const Rot3AxisType, const float& xAngle, const float& yAngle, const float& zAngle)
{
	this->set(ROT_X, xAngle);

	Matrix temp(ROT_Y, yAngle);
	*this *= temp;

	temp.set(ROT_Z, zAngle);
	*this *= temp;
}

void Matrix::set(const RotAxisAngleType, const Vect& axis, const float& angle)
{
	this->set(Quat(ROT_AXIS_ANGLE, axis, angle));
}

void Matrix::set(const MatrixRowType row, const Vect& v)
{
	switch(row)
	{
	case ROW_0:
		this->v0 = v;
		break;

	case ROW_1:
		this->v1 = v;
		break;

	case ROW_2:
		this->v2 = v;
		break;

	case ROW_3:
		this->v3 = v;
		break;

	default:
		GameAssert(0);
		break;
	}
}

void Matrix::set(const Vect& v0, const Vect& v1, const Vect& v2, const Vect& v3)
{
	this->v0 = v0;
	this->v1 = v1;
	this->v2 = v2;
	this->v3 = v3;
}

void Matrix::set(const Quat& quat)
{
	this->v0 = -IDENTITY_ROW_0;
	this->v1 = -IDENTITY_ROW_1;
	this->v2 = -IDENTITY_ROW_2;
	this->v3 = IDENTITY_ROW_3;

#ifdef MATH_SIMD
	M128_TYPE a = _mm_setr_ps(quat.qy, quat.qx, quat.qx, 0.0f);
	M128_TYPE b = _mm_setr_ps(quat.qy, quat.qy, quat.qz, 0.0f);
	M128_TYPE c = _mm_setr_ps(quat.qz, quat.qw, -quat.qw, 0.0f);
	M128_TYPE d = _mm_setr_ps(quat.qz, quat.qz, quat.qy, 0.0f);

	M128_TYPE e = _mm_setr_ps(quat.qx, quat.qx, quat.qy, 0.0f);
	M128_TYPE f = _mm_setr_ps(quat.qy, quat.qx, quat.qz, 0.0f);
	M128_TYPE g = _mm_setr_ps(-quat.qw, quat.qz, quat.qw, 0.0f);
	M128_TYPE h = _mm_setr_ps(quat.qz, quat.qz, quat.qx, 0.0f);

	M128_TYPE i = _mm_setr_ps(quat.qx, quat.qy, quat.qx, 0.0f);
	M128_TYPE j = _mm_setr_ps(quat.qz, quat.qz, quat.qx, 0.0f);
	M128_TYPE k = _mm_setr_ps(quat.qw, -quat.qw, quat.qy, 0.0f);
	M128_TYPE l = _mm_setr_ps(quat.qy, quat.qx, quat.qy, 0.0f);

	M128_TYPE two = _mm_set1_ps(2.0f);

#else // MATH_NO_SIMD
	M128_TYPE a(quat.qy, quat.qx, quat.qx, 0.0f);
	M128_TYPE b(quat.qy, quat.qy, quat.qz, 0.0f);
	M128_TYPE c(quat.qz, quat.qw, -quat.qw, 0.0f);
	M128_TYPE d(quat.qz, quat.qz, quat.qy, 0.0f);

	M128_TYPE e(quat.qx, quat.qx, quat.qy, 0.0f);
	M128_TYPE f(quat.qy, quat.qx, quat.qz, 0.0f);
	M128_TYPE g(-quat.qw, quat.qz, quat.qw, 0.0f);
	M128_TYPE h(quat.qz, quat.qz, quat.qx, 0.0f);

	M128_TYPE i(quat.qx, quat.qy, quat.qx, 0.0f);
	M128_TYPE j(quat.qz, quat.qz, quat.qx, 0.0f);
	M128_TYPE k(quat.qw, -quat.qw, quat.qy, 0.0f);
	M128_TYPE l(quat.qy, quat.qx, quat.qy, 0.0f);

	M128_TYPE two(2.0f);

#endif

	this->v0 += two * (a * b + c * d);
	this->m0 *= -1.0;

	this->v1 += two * (e * f + g * h);
	this->m5 *= -1.0f;

	this->v2 += two * (i * j + k * l);
	this->m10 *= -1.0f;

	this->m3 = 0.0f;
	this->m7 = 0.0f;
	this->m11 = 0.0f;
}

const Vect Matrix::get(const MatrixRowType row) const
{
	switch(row)
	{
	case ROW_0:
		return this->v0;

	case ROW_1:
		return this->v1;

	case ROW_2:
		return this->v2;

	case ROW_3:
		return this->v3;

	default:
		GameAssert(0);
		return Vect();
	}
}

const float Matrix::operator[](const m0_enum) const
{
	return this->m0;
}

const float Matrix::operator[](const m1_enum) const
{
	return this->m1;
}

const float Matrix::operator[](const m2_enum) const
{
	return this->m2;
}

const float Matrix::operator[](const m3_enum) const
{
	return this->m3;
}

const float Matrix::operator[](const m4_enum) const
{
	return this->m4;
}

const float Matrix::operator[](const m5_enum) const
{
	return this->m5;
}

const float Matrix::operator[](const m6_enum) const
{
	return this->m6;
}

const float Matrix::operator[](const m7_enum) const
{
	return this->m7;
}

const float Matrix::operator[](const m8_enum) const
{
	return this->m8;
}

const float Matrix::operator[](const m9_enum) const
{
	return this->m9;
}

const float Matrix::operator[](const m10_enum) const
{
	return this->m10;
}

const float Matrix::operator[](const m11_enum) const
{
	return this->m11;
}

const float Matrix::operator[](const m12_enum) const
{
	return this->m12;
}

const float Matrix::operator[](const m13_enum) const
{
	return this->m13;
}

const float Matrix::operator[](const m14_enum) const
{
	return this->m14;
}

const float Matrix::operator[](const m15_enum) const
{
	return this->m15;
}

float& Matrix::operator[](const m0_enum)
{
	return this->m0;
}

float& Matrix::operator[](const m1_enum)
{
	return this->m1;
}

float& Matrix::operator[](const m2_enum)
{
	return this->m2;
}

float& Matrix::operator[](const m3_enum)
{
	return this->m3;
}

float& Matrix::operator[](const m4_enum)
{
	return this->m4;
}

float& Matrix::operator[](const m5_enum)
{
	return this->m5;
}

float& Matrix::operator[](const m6_enum)
{
	return this->m6;
}

float& Matrix::operator[](const m7_enum)
{
	return this->m7;
}

float& Matrix::operator[](const m8_enum)
{
	return this->m8;
}

float& Matrix::operator[](const m9_enum)
{
	return this->m9;
}

float& Matrix::operator[](const m10_enum)
{
	return this->m10;
}

float& Matrix::operator[](const m11_enum)
{
	return this->m11;
}

float& Matrix::operator[](const m12_enum)
{
	return this->m12;
}

float& Matrix::operator[](const m13_enum)
{
	return this->m13;
}

float& Matrix::operator[](const m14_enum)
{
	return this->m14;
}

float& Matrix::operator[](const m15_enum)
{
	return this->m15;
}

const Matrix Matrix::operator+(const Matrix& mtx) const
{
	M128_TYPE row0 = this->v0.m + mtx.v0.m;
	M128_TYPE row1 = this->v1.m + mtx.v1.m;
	M128_TYPE row2 = this->v2.m + mtx.v2.m;
	M128_TYPE row3 = this->v3.m + mtx.v3.m;

	return Matrix(row0, row1, row2, row3);
}

const Matrix Matrix::operator-(const Matrix& mtx) const
{
	M128_TYPE row0 = this->v0.m - mtx.v0.m;
	M128_TYPE row1 = this->v1.m - mtx.v1.m;
	M128_TYPE row2 = this->v2.m - mtx.v2.m;
	M128_TYPE row3 = this->v3.m - mtx.v3.m;

	return Matrix(row0, row1, row2, row3);
}

void Matrix::operator+=(const Matrix& mtx)
{
	this->v0.m += mtx.v0.m;
	this->v1.m += mtx.v1.m;
	this->v2.m += mtx.v2.m;
	this->v3.m += mtx.v3.m;
}

void Matrix::operator-=(const Matrix& mtx)
{
	this->v0.m -= mtx.v0.m;
	this->v1.m -= mtx.v1.m;
	this->v2.m -= mtx.v2.m;
	this->v3.m -= mtx.v3.m;
}

const Matrix operator*(const float a, const Matrix& mtx)
{
	return mtx * a;
}

const Matrix Matrix::operator*(const float a) const
{
#ifdef MATH_SIMD
	M128_TYPE scalar = _mm_set1_ps(a);

#else // MATH_NO_SIMD
	M128_TYPE scalar(a);

#endif

	M128_TYPE row0 = this->v0.m * scalar;
	M128_TYPE row1 = this->v1.m * scalar;
	M128_TYPE row2 = this->v2.m * scalar;
	M128_TYPE row3 = this->v3.m * scalar;

	return Matrix(row0, row1, row2, row3);
}

void Matrix::operator*=(const float a)
{

#ifdef MATH_SIMD
	M128_TYPE scalar = _mm_set1_ps(a);

#else
	M128_TYPE scalar(a);

#endif

	this->v0.m *= scalar;
	this->v1.m *= scalar;
	this->v2.m *= scalar;
	this->v3.m *= scalar;
}

const Matrix Matrix::operator*(const Matrix& mtx) const
{
	return Matrix(*this) *= mtx;
}

Matrix& Matrix::operator*=(const Matrix& mtx)
{

#ifdef MATH_SIMD
	M128_TYPE col0 = _mm_setr_ps(mtx.m0, mtx.m4, mtx.m8, mtx.m12);
	M128_TYPE col1 = _mm_setr_ps(mtx.m1, mtx.m5, mtx.m9, mtx.m13);
	M128_TYPE col2 = _mm_setr_ps(mtx.m2, mtx.m6, mtx.m10, mtx.m14);
	M128_TYPE col3 = _mm_setr_ps(mtx.m3, mtx.m7, mtx.m11, mtx.m15);

#else // MATH_NO_SIMD
	M128_TYPE col0(mtx.m0, mtx.m4, mtx.m8, mtx.m12);
	M128_TYPE col1(mtx.m1, mtx.m5, mtx.m9, mtx.m13);
	M128_TYPE col2(mtx.m2, mtx.m6, mtx.m10, mtx.m14);
	M128_TYPE col3(mtx.m3, mtx.m7, mtx.m11, mtx.m15);

#endif

	M128_TYPE row0 = this->v0.m;
	M128_TYPE row1 = this->v1.m;
	M128_TYPE row2 = this->v2.m;
	M128_TYPE row3 = this->v3.m;

	_dot_product_ps(row0, col0, this->m0);
	_dot_product_ps(row0, col1, this->m1);
	_dot_product_ps(row0, col2, this->m2);
	_dot_product_ps(row0, col3, this->m3);

	_dot_product_ps(row1, col0, this->m4);
	_dot_product_ps(row1, col1, this->m5);
	_dot_product_ps(row1, col2, this->m6);
	_dot_product_ps(row1, col3, this->m7);

	_dot_product_ps(row2, col0, this->m8);
	_dot_product_ps(row2, col1, this->m9);
	_dot_product_ps(row2, col2, this->m10);
	_dot_product_ps(row2, col3, this->m11);

	_dot_product_ps(row3, col0, this->m12);
	_dot_product_ps(row3, col1, this->m13);
	_dot_product_ps(row3, col2, this->m14);
	_dot_product_ps(row3, col3, this->m15);

	return *this;
}

const Matrix Matrix::operator+() const
{
	return *this;
}

const Matrix Matrix::operator-() const
{
	return *this * -1.0f;
}

const float Matrix::det() const
{

#ifdef MATH_SIMD
	M128_TYPE a = this->v0.m;
	M128_TYPE b = _mm_setr_ps(m5, m4, m4, m4);
	M128_TYPE c = _mm_setr_ps(m10, m10, m9, m9);
	M128_TYPE d = _mm_setr_ps(m15, m15, m15, m14);
	M128_TYPE e = _mm_setr_ps(m11, m11, m11, m10);
	M128_TYPE f = _mm_setr_ps(m14, m14, m13, m13);
	M128_TYPE g = _mm_setr_ps(m6, m6, m5, m5);
	M128_TYPE h = _mm_setr_ps(m9, m8, m8, m8);
	M128_TYPE i = d;
	M128_TYPE j = e;
	M128_TYPE k = _mm_setr_ps(m13, m12, m12, m12);
	M128_TYPE l = _mm_setr_ps(m7, m7, m7, m6);
	M128_TYPE m = h;
	M128_TYPE n = f;
	M128_TYPE o = c;
	M128_TYPE p = k;

	const static M128_TYPE DETERMINANT_COEFF = _mm_setr_ps(1.0f, -1.0f, 1.0f, -1.0f);

#else // MATH_NO_SIMD
	M128_TYPE a = this->v0.m;
	M128_TYPE b(m5, m4, m4, m4);
	M128_TYPE c(m10, m10, m9, m9);
	M128_TYPE d(m15, m15, m15, m14);
	M128_TYPE e(m11, m11, m11, m10);
	M128_TYPE f(m14, m14, m13, m13);
	M128_TYPE g(m6, m6, m5, m5);
	M128_TYPE h(m9, m8, m8, m8);
	M128_TYPE i = d;
	M128_TYPE j = e;
	M128_TYPE k(m13, m12, m12, m12);
	M128_TYPE l(m7, m7, m7, m6);
	M128_TYPE m = h;
	M128_TYPE n = f;
	M128_TYPE o = c;
	M128_TYPE p = k;

	const static M128_TYPE DETERMINANT_COEFF(1.0f, -1.0f, 1.0f, -1.0f);
#endif

	M128_TYPE result = a*(b*(c*d - e*f) - g*(h*i - j*k) + l*(m*n - o*p));
	float det;
	_dot_product_ps(result, DETERMINANT_COEFF, det);

	return det;
}

Matrix& Matrix::T()
{

#ifdef MATH_SIMD
	M128_TYPE col0 = _mm_setr_ps(m0, m4, m8, m12);
	M128_TYPE col1 = _mm_setr_ps(m1, m5, m9, m13);
	M128_TYPE col2 = _mm_setr_ps(m2, m6, m10, m14);
	M128_TYPE col3 = _mm_setr_ps(m3, m7, m11, m15);

#else
	M128_TYPE col0(m0, m4, m8, m12);
	M128_TYPE col1(m1, m5, m9, m13);
	M128_TYPE col2(m2, m6, m10, m14);
	M128_TYPE col3(m3, m7, m11, m15);

#endif

	this->v0.m = col0;
	this->v1.m = col1;
	this->v2.m = col2;
	this->v3.m = col3;

	return *this;
}

const Matrix Matrix::getT() const
{
	return Matrix(*this).T();
}

Matrix& Matrix::inv()
{
	float determinant = this->det();

	float b0 = m6*m11*m13 - m7*m10*m13 + m7*m9*m14 - m5*m11*m14 - m6*m9*m15 + m5*m10*m15;
	float b1 = m3*m10*m13 - m2*m11*m13 - m3*m9*m14 + m1*m11*m14 + m2*m9*m15 - m1*m10*m15;
	float b2 = m2*m7*m13 - m3*m6*m13 + m3*m5*m14 - m1*m7*m14 - m2*m5*m15 + m1*m6*m15;
	float b3 = m3*m6*m9 - m2*m7*m9 - m3*m5*m10 + m1*m7*m10 + m2*m5*m11 - m1*m6*m11;
	float b4 = m7*m10*m12 - m6*m11*m12 - m7*m8*m14 + m4*m11*m14 + m6*m8*m15 - m4*m10*m15;
	float b5 = m2*m11*m12 - m3*m10*m12 + m3*m8*m14 - m0*m11*m14 - m2*m8*m15 + m0*m10*m15;
	float b6 = m3*m6*m12 - m2*m7*m12 - m3*m4*m14 + m0*m7*m14 + m2*m4*m15 - m0*m6*m15;
	float b7 = m2*m7*m8 - m3*m6*m8 + m3*m4*m10 - m0*m7*m10 - m2*m4*m11 + m0*m6*m11;
	float b8 = m5*m11*m12 - m7*m9*m12 + m7*m8*m13 - m4*m11*m13 - m5*m8*m15 + m4*m9*m15;
	float b9 = m3*m9*m12 - m1*m11*m12 - m3*m8*m13 + m0*m11*m13 + m1*m8*m15 - m0*m9*m15;
	float b10 = m1*m7*m12 - m3*m5*m12 + m3*m4*m13 - m0*m7*m13 - m1*m4*m15 + m0*m5*m15;
	float b11 = m3*m5*m8 - m1*m7*m8 - m3*m4*m9 + m0*m7*m9 + m1*m4*m11 - m0*m5*m11;
	float b12 = m6*m9*m12 - m5*m10*m12 - m6*m8*m13 + m4*m10*m13 + m5*m8*m14 - m4*m9*m14;
	float b13 = m1*m10*m12 - m2*m9*m12 + m2*m8*m13 - m0*m10*m13 - m1*m8*m14 + m0*m9*m14;
	float b14 = m2*m5*m12 - m1*m6*m12 - m2*m4*m13 + m0*m6*m13 + m1*m4*m14 - m0*m5*m14;
	float b15 = m1*m6*m8 - m2*m5*m8 + m2*m4*m9 - m0*m6*m9 - m1*m4*m10 + m0*m5*m10;

	this->m0 = b0;
	this->m1 = b1;
	this->m2 = b2;
	this->m3 = b3;
	this->m4 = b4;
	this->m5 = b5;
	this->m6 = b6;
	this->m7 = b7;
	this->m8 = b8;
	this->m9 = b9;
	this->m10 = b10;
	this->m11 = b11;
	this->m12 = b12;
	this->m13 = b13;
	this->m14 = b14;
	this->m15 = b15;

	(*this) *= (1 / determinant);

	return *this;
}

const Matrix Matrix::getInv() const
{
	return Matrix(*this).inv();
}