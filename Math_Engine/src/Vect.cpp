#include <GameAssert.h>
#include "MathEngine.h"

#ifdef MATH_SIMD
const static __m128 DEFAULT_VECTOR_VALUES = _mm_setr_ps(0.0f, 0.0f, 0.0f, 1.0f);
const static __m128 NEGATION_TERM = _mm_set1_ps(-1.0f);

#else // MATH_NO_SIMD
const static Math128 DEFAULT_VECTOR_VALUES( 0.0f, 0.0f, 0.0f, 1.0f );
const static Math128 NEGATION_TERM( -1.0f );

#endif



Vect::Vect()
	: m( DEFAULT_VECTOR_VALUES )
{
	CHECK_ALIGNMENT_16( this );
}

Vect::Vect( const M128_TYPE& _m )
	: m( _m )
{
	CHECK_ALIGNMENT_16( this );
}

Vect::Vect( const M128_TYPE& _m, float w )
	: m( _m )
{
	CHECK_ALIGNMENT_16( this );

	this->w = w;
}

Vect::Vect( const Vect& v, const NormType& )
	: m( v.m )
{
	CHECK_ALIGNMENT_16( this );

	this->norm();
}


Vect::Vect( const Vect& v )
	: m( v.m )
{
	CHECK_ALIGNMENT_16( this );
}

Vect::Vect( const Vect& v, float w )
	: m( v.m )
{
	CHECK_ALIGNMENT_16( this );

	this->w = w;
}

Vect::Vect( float _x, float _y, float _z, float _w )
	: x( _x ),
	y( _y ),
	z( _z ),
	w( _w )
{
	CHECK_ALIGNMENT_16( this );
}

Vect& Vect::operator=( const Vect& v )
{
	this->m = v.m;
	return *this;
}

Vect::~Vect()
{
	// Do nothing
}

bool Vect::isEqual( const Vect& rhs, float tolerance ) const
{
	M128_TYPE result = _mm_abs_ps( this->m - rhs.m );

	return ( result.m128_f32[0] < tolerance &&
			 result.m128_f32[1] < tolerance &&
			 result.m128_f32[2] < tolerance &&
			 result.m128_f32[3] < tolerance );
}

bool Vect::isZero( float tolerance ) const
{
	M128_TYPE result = _mm_abs_ps( this->m - DEFAULT_VECTOR_VALUES );

	return ( result.m128_f32[0] < tolerance &&
			 result.m128_f32[1] < tolerance &&
			 result.m128_f32[2] < tolerance &&
			 result.m128_f32[3] < tolerance );
}

void Vect::set( float x, float y, float z, float w )
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

void Vect::set( const Vect& v )
{
	this->m = v.m;
}

Vect Vect::operator+( const Vect& rhs ) const
{
	return Vect( this->m + rhs.m, 1.0f );
}

Vect Vect::operator-( const Vect& rhs ) const
{
	return Vect( this->m - rhs.m, 1.0f );
}

void Vect::operator+=( const Vect& rhs )
{
	this->m += rhs.m;
	this->w = 1.0f;
}

void Vect::operator-=( const Vect& rhs )
{
	this->m -= rhs.m;
	this->w = 1.0f;
}

float Vect::dot( const Vect& v2 ) const
{
	M128_TYPE squared = this->m * v2.m;
	float result = squared.m128_f32[0];
	result += squared.m128_f32[1];
	result += squared.m128_f32[2];

	return result;
}

Vect Vect::cross( const Vect& v2 ) const
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
	M128_TYPE col0( this->y, this->z, this->x, 1.0f );
	M128_TYPE col1( v2.z, v2.x, v2.y, 1.0f );
	M128_TYPE col2( this->z, this->x, this->y, 0.0f );
	M128_TYPE col3( v2.y, v2.z, v2.x, 0.0f );
#endif

	col0 *= col1;
	col2 *= col3;
	return Vect( col0 - col2 );
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
	GameAssert( length != 0.0f );

	length = 1.0f / length;

	this->m *= length;

#endif


	this->w = 1.0f;
}

Vect Vect::getNorm() const
{
	return Vect( *this, NORMALIZE );
}

float Vect::mag() const
{
	M128_TYPE squaredLength = this->m * this->m;
	return sqrtf( squaredLength.m128_f32[0] + squaredLength.m128_f32[1] + squaredLength.m128_f32[2] );
}

float Vect::magSqr() const
{
	M128_TYPE squaredLength = this->m * this->m;
	return squaredLength.m128_f32[0] + squaredLength.m128_f32[1] + squaredLength.m128_f32[2];
}

float Vect::getAngle( const Vect& v2 ) const
{
	float dotProduct = this->dot( v2 );
	dotProduct /= this->mag();
	dotProduct /= v2.mag();

	return acosf( dotProduct );
}

Vect operator*( float a, const Vect& b )
{
	return b * a;
}

Vect Vect::operator*( float a ) const
{
	return Vect( this->m * a, 1.0f );
}

void Vect::operator*=( float a )
{
	this->m *= a;
	this->w = 1.0f;
}

Vect Vect::operator+( ) const
{
	return Vect( *this, 1.0f );
}

Vect Vect::operator-( ) const
{
	M128_TYPE negated = this->m * NEGATION_TERM;
	negated.m128_f32[3] = 1.0f;
	return Vect( negated );
}

Vect Vect::operator*( const Matrix& mtx ) const
{
	return Vect( *this ) *= mtx;
}

Vect& Vect::operator*=( const Matrix& mtx )
{
#ifdef MATH_SIMD
	M128_TYPE col0 = _mm_setr_ps(mtx.v0[X], mtx.v1[X], mtx.v2[X], mtx.v3[X]);
	M128_TYPE col1 = _mm_setr_ps(mtx.v0[Y], mtx.v1[Y], mtx.v2[Y], mtx.v3[Y]);
	M128_TYPE col2 = _mm_setr_ps(mtx.v0[Z], mtx.v1[Z], mtx.v2[Z], mtx.v3[Z]);
	M128_TYPE col3 = _mm_setr_ps(mtx.v0[W], mtx.v1[W], mtx.v2[W], mtx.v3[W]);

#else // MATH_NO_SIMD
	M128_TYPE col0( mtx.v0[X], mtx.v1[X], mtx.v2[X], mtx.v3[X] );
	M128_TYPE col1( mtx.v0[Y], mtx.v1[Y], mtx.v2[Y], mtx.v3[Y] );
	M128_TYPE col2( mtx.v0[Z], mtx.v1[Z], mtx.v2[Z], mtx.v3[Z] );
	M128_TYPE col3( mtx.v0[W], mtx.v1[W], mtx.v2[W], mtx.v3[W] );
#endif

	M128_TYPE v = this->m;

	_dot_product_ps( v, col0, this->x );
	_dot_product_ps( v, col1, this->y );
	_dot_product_ps( v, col2, this->z );
	_dot_product_ps( v, col3, this->w );

	return *this;
}