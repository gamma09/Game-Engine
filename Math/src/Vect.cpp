#include <GameAssert.h>
#include "MathEngine.h"

#ifdef MATH_SIMD
	const static __m128 DEFAULT_VECTOR_VALUES = _mm_setr_ps(0.0f, 0.0f, 0.0f, 1.0f);
	const static __m128 NEGATION_TERM = _mm_set1_ps(-1.0f);

#else // MATH_NO_SIMD
	const static Math128 DEFAULT_VECTOR_VALUES(0.0f, 0.0f, 0.0f, 1.0f);
	const static Math128 NEGATION_TERM(-1.0f);

#endif



Vect::Vect() :
	m(DEFAULT_VECTOR_VALUES)
{
	CHECK_ALIGNMENT_16(this);
}

Vect::Vect(const M128_TYPE& _m) :
	m(_m)
{
	CHECK_ALIGNMENT_16(this);
}

Vect::Vect(const M128_TYPE& _m, const float w) :
	m(_m)
{
	CHECK_ALIGNMENT_16(this);

	this->w = w;
}

Vect::Vect(const Vect& v, const NormType&) :
	m(v.m)
{
	CHECK_ALIGNMENT_16(this);

	this->norm();
}


Vect::Vect(const Vect& v) :
	m(v.m)
{
	CHECK_ALIGNMENT_16(this);
}

Vect::Vect(const Vect& v, const float w) :
	m(v.m)
{
	CHECK_ALIGNMENT_16(this);

	this->w = w;
}

Vect::Vect(const float _x, const float _y, const float _z, const float _w) :
	x(_x),
	y(_y),
	z(_z),
	w(_w)
{
	CHECK_ALIGNMENT_16(this);
}

Vect& Vect::operator=(const Vect& v)
{
	this->m = v.m;
	return *this;
}

Vect::~Vect()
{
	// Do nothing
}

const bool Vect::isEqual(const Vect& rhs, const float tolerance) const
{
	M128_TYPE result = _mm_abs_ps(this->m - rhs.m);

	return (result.m128_f32[0] < tolerance &&
			result.m128_f32[1] < tolerance &&
			result.m128_f32[2] < tolerance &&
			result.m128_f32[3] < tolerance);
}

const bool Vect::isZero(const float tolerance) const
{
	M128_TYPE result = _mm_abs_ps(this->m - DEFAULT_VECTOR_VALUES);

	return (result.m128_f32[0] < tolerance &&
			result.m128_f32[1] < tolerance &&
			result.m128_f32[2] < tolerance &&
			result.m128_f32[3] < tolerance);
}

void Vect::set(const float x, const float y, const float z, const float w)
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

void Vect::set(const Vect& v)
{
	this->m = v.m;
}

const float Vect::operator[] (const x_enum) const
{
	return this->x;
}

const float Vect::operator[] (const y_enum) const
{
	return this->y;
}

const float Vect::operator[] (const z_enum) const
{
	return this->z;
}

const float Vect::operator[] (const w_enum) const
{
	return this->w;
}

float& Vect::operator[] (const x_enum)
{
	return this->x;
}

float& Vect::operator[] (const y_enum)
{
	return this->y;
}

float& Vect::operator[] (const z_enum)
{
	return this->z;
}

float& Vect::operator[] (const w_enum)
{
	return this->w;
}

const Vect Vect::operator+(const Vect& rhs) const
{
	return Vect(this->m + rhs.m, 1.0f);
}

const Vect Vect::operator-(const Vect& rhs) const
{
	return Vect(this->m - rhs.m, 1.0f);
}

void Vect::operator+=(const Vect& rhs)
{
	this->m += rhs.m;
	this->w = 1.0f;
}

void Vect::operator-=(const Vect& rhs)
{
	this->m -= rhs.m;
	this->w = 1.0f;
}

const float Vect::dot(const Vect& v2) const
{
	M128_TYPE squared = this->m * v2.m;
	float result = squared.m128_f32[0];
	result += squared.m128_f32[1];
	result += squared.m128_f32[2];
	
	return result;
}

const Vect Vect::cross(const Vect& v2) const
{
	//     col0   col1   col2   col3
	// x = v1.y * v2.z - v1.z * v2.y
	// y = v1.z * v2.x - v1.x * v2.z
	// z = v1.x * v2.y - v1.y * v2.x
	
	#ifdef MATH_SIMD
		M128_TYPE col0 = _mm_setr_ps(this->y, this->z, this->x, 1.0f);
		M128_TYPE col1 = _mm_setr_ps(v2.z, v2.x, v2.y, 1.0f);
		M128_TYPE col2 = _mm_setr_ps(this->z, this->x, this->y, 0.0f);
		M128_TYPE col3 = _mm_setr_ps(v2.y, v2.z, v2.x, 0.0f);
	#else // MATH_NO_SIMD
		M128_TYPE col0(this->y, this->z, this->x, 1.0f);
		M128_TYPE col1(v2.z, v2.x, v2.y, 1.0f);
		M128_TYPE col2(this->z, this->x, this->y, 0.0f);
		M128_TYPE col3(v2.y, v2.z, v2.x, 0.0f);
	#endif

	col0 *= col1;
	col2 *= col3;
	return Vect(col0 - col2);
}

void Vect::norm()
{
	#ifdef MATH_SIMD
		M128_TYPE squaredLength = _mm_set1_ps(this->magSqr());
		GameAssert(squaredLength.m128_f32[0] != 0.0f);

		M128_TYPE inverseRoot = _mm_rsqrt_ps(squaredLength);
		this->m *= inverseRoot;
	
	#else // MATH_NO_SIMD

		float length = this->mag();
		GameAssert(length != 0.0f);

		length = 1.0f / length;

		this->m *= length;

	#endif


	this->w = 1.0f;
}

const Vect Vect::getNorm() const
{
	return Vect(*this, NORMALIZE);
}

const float Vect::mag() const
{
	M128_TYPE squaredLength = this->m * this->m;
	return sqrtf(squaredLength.m128_f32[0] + squaredLength.m128_f32[1] + squaredLength.m128_f32[2]);
}

const float Vect::magSqr() const
{
	M128_TYPE squaredLength = this->m * this->m;
	return squaredLength.m128_f32[0] + squaredLength.m128_f32[1] + squaredLength.m128_f32[2];
}

const float Vect::getAngle(const Vect& v2) const
{
	float dotProduct = this->dot(v2);
	dotProduct /= this->mag();
	dotProduct /= v2.mag();
	
	return acosf(dotProduct);
}

const Vect operator*(const float a, const Vect& b)
{
	return b * a;
}

const Vect Vect::operator*(const float a) const
{
	return Vect(this->m * a, 1.0f);
}

void Vect::operator*=(const float a)
{
	this->m *= a;
	this->w = 1.0f;
}

const Vect Vect::operator+() const
{
	return Vect(*this, 1.0f);
}

const Vect Vect::operator-() const
{
	M128_TYPE negated = this->m * NEGATION_TERM;
	negated.m128_f32[3] = 1.0f;
	return Vect(negated);
}

const Vect Vect::operator*(const Matrix& mtx) const
{
	return Vect(*this) *= mtx;
}

Vect& Vect::operator*=(const Matrix& mtx)
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

	M128_TYPE v = this->m;

	_dot_product_ps(v, col0, this->x);
	_dot_product_ps(v, col1, this->y);
	_dot_product_ps(v, col2, this->z);
	_dot_product_ps(v, col3, this->w);

	return *this;
}