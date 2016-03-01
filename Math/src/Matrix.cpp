#include "Matrix.h"
#include <math.h>
#include <GameAssert.h>

#include "Quat.h"

const static Vect ZERO_ROW( 0.0f, 0.0f, 0.0f, 0.0f );

const static Vect IDENTITY_ROW_0( 1.0f, 0.0f, 0.0f, 0.0f );
const static Vect IDENTITY_ROW_1( 0.0f, 1.0f, 0.0f, 0.0f );
const static Vect IDENTITY_ROW_2( 0.0f, 0.0f, 1.0f, 0.0f );
const static Vect IDENTITY_ROW_3( 0.0f, 0.0f, 0.0f, 1.0f );

Matrix::Matrix() :
v0( ZERO_ROW ),
v1( ZERO_ROW ),
v2( ZERO_ROW ),
v3( ZERO_ROW )
{
	CHECK_ALIGNMENT_16( this );
}

Matrix::Matrix( const Matrix& mtx ) :
v0( mtx.v0 ),
v1( mtx.v1 ),
v2( mtx.v2 ),
v3( mtx.v3 )
{
	CHECK_ALIGNMENT_16( this );
}

Matrix::Matrix( const Vect& _v0, const Vect& _v1, const Vect& _v2, const Vect& _v3 ) :
v0( _v0 ),
v1( _v1 ),
v2( _v2 ),
v3( _v3 )
{
	CHECK_ALIGNMENT_16( this );
}

Matrix::Matrix( const M128_TYPE& row0, const M128_TYPE& row1, const M128_TYPE& row2, const M128_TYPE& row3 ) :
v0( row0 ),
v1( row1 ),
v2( row2 ),
v3( row3 )
{
	CHECK_ALIGNMENT_16( this );
}

Matrix::Matrix( const MatrixSpecialType type )
{
	CHECK_ALIGNMENT_16( this );

	this->set( type );
}

Matrix::Matrix( const MatrixTransType, const float x, const float y, const float z ) :
v0( IDENTITY_ROW_0 ),
v1( IDENTITY_ROW_1 ),
v2( IDENTITY_ROW_2 ),
v3( x, y, z )
{
	CHECK_ALIGNMENT_16( this );
}

Matrix::Matrix( const MatrixTransType, const Vect& trans ) :
v0( IDENTITY_ROW_0 ),
v1( IDENTITY_ROW_1 ),
v2( IDENTITY_ROW_2 ),
v3( trans )
{
	CHECK_ALIGNMENT_16( this );
}

Matrix::Matrix( const MatrixScaleType, const float sx, const float sy, const float sz )
{
	CHECK_ALIGNMENT_16( this );

	this->set( SCALE, sx, sy, sz );
}

Matrix::Matrix( const MatrixScaleType, const Vect& scale )
{
	CHECK_ALIGNMENT_16( this );

	this->set( SCALE, scale );
}

Matrix::Matrix( const RotOrientType type, const Vect& direction, const Vect& up ) :
v3()
{
	CHECK_ALIGNMENT_16( this );

	this->set( type, direction, up );
}

Matrix::Matrix( const RotAxisAngleType, const Vect& axis, const float& angle )
{
	CHECK_ALIGNMENT_16( this );

	this->set( ROT_AXIS_ANGLE, axis, angle );
}

Matrix::Matrix( const Rot3AxisType, const float& xAngle, const float& yAngle, const float& zAngle )
{
	CHECK_ALIGNMENT_16( this );

	this->set( ROT_XYZ, xAngle, yAngle, zAngle );
}

Matrix::Matrix( const Quat& quat )
{
	CHECK_ALIGNMENT_16( this );

	this->set( quat );
}

Matrix::Matrix( const RotType type, const float angle )
{
	CHECK_ALIGNMENT_16( this );

	this->set( type, angle );
}

Matrix& Matrix::operator=( const Matrix& mtx )
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

const bool Matrix::isEqual( const Matrix& mtx, const float tolerance ) const
{
	return ( this->v0.isEqual( mtx.v0, tolerance ) &&
			 this->v1.isEqual( mtx.v1, tolerance ) &&
			 this->v2.isEqual( mtx.v2, tolerance ) &&
			 this->v3.isEqual( mtx.v3, tolerance ) );
}

const bool Matrix::isIdentity( const float tolerance ) const
{
	return ( this->v0.isEqual( IDENTITY_ROW_0, tolerance ) &&
			 this->v1.isEqual( IDENTITY_ROW_1, tolerance ) &&
			 this->v2.isEqual( IDENTITY_ROW_2, tolerance ) &&
			 this->v3.isEqual( IDENTITY_ROW_3, tolerance ) );
}

void Matrix::set( const MatrixSpecialType type )
{
	switch( type )
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
		GameAssert( 0 );
		break;
	}
}

void Matrix::set( const MatrixTransType, const float x, const float y, const float z )
{
	this->v0 = IDENTITY_ROW_0;
	this->v1 = IDENTITY_ROW_1;
	this->v2 = IDENTITY_ROW_2;
	this->v3.set( x, y, z, 1.0f );
}

void Matrix::set( const MatrixTransType, const Vect& v )
{
	this->v0 = IDENTITY_ROW_0;
	this->v1 = IDENTITY_ROW_1;
	this->v2 = IDENTITY_ROW_2;
	this->v3 = Vect( v, 1.0f );
}


void Matrix::set( const MatrixScaleType, const float sx, const float sy, const float sz )
{
	this->v0 = ZERO_ROW;
	this->v1 = ZERO_ROW;
	this->v2 = ZERO_ROW;
	this->v3 = IDENTITY_ROW_3;

	this->v0[X] = sx;
	this->v1[Y] = sy;
	this->v2[Z] = sz;
}

void Matrix::set( const MatrixScaleType, const Vect& scale )
{
	this->v0 = ZERO_ROW;
	this->v1 = ZERO_ROW;
	this->v2 = ZERO_ROW;
	this->v3 = IDENTITY_ROW_3;

	this->v0[X] = scale.x;
	this->v1[Y] = scale.y;
	this->v2[Z] = scale.z;
}

void Matrix::set( const RotType type, const float angle )
{
	float cs = cosf( angle );
	float sn = sinf( angle );

	switch( type )
	{
	case ROT_X:
		this->v0 = IDENTITY_ROW_0;
		this->v1.set( 0.0f, cs, sn, 0.0f );
		this->v2.set( 0.0f, -sn, cs, 0.0f );
		break;

	case ROT_Y:
		this->v0.set( cs, 0.0f, -sn, 0.0f );
		this->v1 = IDENTITY_ROW_1;
		this->v2.set( sn, 0.0f, cs, 0.0f );
		break;

	case ROT_Z:
		this->v0.set( cs, sn, 0.0f, 0.0f );
		this->v1.set( -sn, cs, 0.0f, 0.0f );
		this->v2 = IDENTITY_ROW_2;
		break;

	default:
		GameAssert( 0 );
		break;
	}

	this->v3 = IDENTITY_ROW_3;
}

void Matrix::set( const RotOrientType type, const Vect& direction, const Vect& up )
{
	this->v2 = direction.getNorm();
	this->v0 = up.cross( this->v2 );
	this->v0.norm();
	this->v1 = this->v2.cross( this->v0 );
	this->v1.norm();

	this->v0[W] = 0.0f;
	this->v1[W] = 0.0f;
	this->v2[W] = 0.0f;

	this->v3 = IDENTITY_ROW_3;

	switch( type )
	{
	case ROT_ORIENT:
		// Do nothing
		break;

	case ROT_INVERSE_ORIENT:
		this->inv();
		break;

	default:
		GameAssert( 0 );
	}
}

void Matrix::set( const Rot3AxisType, const float& xAngle, const float& yAngle, const float& zAngle )
{
	this->set( ROT_X, xAngle );

	Matrix temp( ROT_Y, yAngle );
	*this *= temp;

	temp.set( ROT_Z, zAngle );
	*this *= temp;
}

void Matrix::set( const RotAxisAngleType, const Vect& axis, const float& angle )
{
	this->set( Quat( ROT_AXIS_ANGLE, axis, angle ) );
}

void Matrix::set( const MatrixRowType row, const Vect& v )
{
	switch( row )
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
		GameAssert( 0 );
		break;
	}
}

void Matrix::set( const Vect& v0, const Vect& v1, const Vect& v2, const Vect& v3 )
{
	this->v0 = v0;
	this->v1 = v1;
	this->v2 = v2;
	this->v3 = v3;
}

void Matrix::set( const Quat& quat )
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
	M128_TYPE a( quat.qy, quat.qx, quat.qx, 0.0f );
	M128_TYPE b( quat.qy, quat.qy, quat.qz, 0.0f );
	M128_TYPE c( quat.qz, quat.qw, -quat.qw, 0.0f );
	M128_TYPE d( quat.qz, quat.qz, quat.qy, 0.0f );

	M128_TYPE e( quat.qx, quat.qx, quat.qy, 0.0f );
	M128_TYPE f( quat.qy, quat.qx, quat.qz, 0.0f );
	M128_TYPE g( -quat.qw, quat.qz, quat.qw, 0.0f );
	M128_TYPE h( quat.qz, quat.qz, quat.qx, 0.0f );

	M128_TYPE i( quat.qx, quat.qy, quat.qx, 0.0f );
	M128_TYPE j( quat.qz, quat.qz, quat.qx, 0.0f );
	M128_TYPE k( quat.qw, -quat.qw, quat.qy, 0.0f );
	M128_TYPE l( quat.qy, quat.qx, quat.qy, 0.0f );

	M128_TYPE two( 2.0f );

#endif

	this->v0 += two * ( a * b + c * d );
	this->v0[X] *= -1.0;

	this->v1 += two * ( e * f + g * h );
	this->v1[Y] *= -1.0f;

	this->v2 += two * ( i * j + k * l );
	this->v2[Z] *= -1.0f;

	this->v0[W] = 0.0f;
	this->v1[W] = 0.0f;
	this->v2[W] = 0.0f;
}

const Vect Matrix::get( const MatrixRowType row ) const
{
	switch( row )
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
		GameAssert( 0 );
		return Vect();
	}
}

const Matrix Matrix::operator+( const Matrix& mtx ) const
{
	M128_TYPE row0 = this->v0.m + mtx.v0.m;
	M128_TYPE row1 = this->v1.m + mtx.v1.m;
	M128_TYPE row2 = this->v2.m + mtx.v2.m;
	M128_TYPE row3 = this->v3.m + mtx.v3.m;

	return Matrix( row0, row1, row2, row3 );
}

const Matrix Matrix::operator-( const Matrix& mtx ) const
{
	M128_TYPE row0 = this->v0.m - mtx.v0.m;
	M128_TYPE row1 = this->v1.m - mtx.v1.m;
	M128_TYPE row2 = this->v2.m - mtx.v2.m;
	M128_TYPE row3 = this->v3.m - mtx.v3.m;

	return Matrix( row0, row1, row2, row3 );
}

void Matrix::operator+=( const Matrix& mtx )
{
	this->v0.m += mtx.v0.m;
	this->v1.m += mtx.v1.m;
	this->v2.m += mtx.v2.m;
	this->v3.m += mtx.v3.m;
}

void Matrix::operator-=( const Matrix& mtx )
{
	this->v0.m -= mtx.v0.m;
	this->v1.m -= mtx.v1.m;
	this->v2.m -= mtx.v2.m;
	this->v3.m -= mtx.v3.m;
}

const Matrix operator*( const float a, const Matrix& mtx )
{
	return mtx * a;
}

const Matrix Matrix::operator*( const float a ) const
{
#ifdef MATH_SIMD
	M128_TYPE scalar = _mm_set1_ps(a);

#else // MATH_NO_SIMD
	M128_TYPE scalar( a );

#endif

	M128_TYPE row0 = this->v0.m * scalar;
	M128_TYPE row1 = this->v1.m * scalar;
	M128_TYPE row2 = this->v2.m * scalar;
	M128_TYPE row3 = this->v3.m * scalar;

	return Matrix( row0, row1, row2, row3 );
}

void Matrix::operator*=( const float a )
{

#ifdef MATH_SIMD
	M128_TYPE scalar = _mm_set1_ps(a);

#else
	M128_TYPE scalar( a );

#endif

	this->v0.m *= scalar;
	this->v1.m *= scalar;
	this->v2.m *= scalar;
	this->v3.m *= scalar;
}

const Matrix Matrix::operator*( const Matrix& mtx ) const
{
	return Matrix( *this ) *= mtx;
}

Matrix& Matrix::operator*=( const Matrix& mtx )
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

	M128_TYPE row0 = this->v0.m;
	M128_TYPE row1 = this->v1.m;
	M128_TYPE row2 = this->v2.m;
	M128_TYPE row3 = this->v3.m;

	_dot_product_ps( row0, col0, this->v0[X] );
	_dot_product_ps( row0, col1, this->v0[Y] );
	_dot_product_ps( row0, col2, this->v0[Z] );
	_dot_product_ps( row0, col3, this->v0[W] );

	_dot_product_ps( row1, col0, this->v1[X] );
	_dot_product_ps( row1, col1, this->v1[Y] );
	_dot_product_ps( row1, col2, this->v1[Z] );
	_dot_product_ps( row1, col3, this->v1[W] );

	_dot_product_ps( row2, col0, this->v2[X] );
	_dot_product_ps( row2, col1, this->v2[Y] );
	_dot_product_ps( row2, col2, this->v2[Z] );
	_dot_product_ps( row2, col3, this->v2[W] );

	_dot_product_ps( row3, col0, this->v3[X] );
	_dot_product_ps( row3, col1, this->v3[Y] );
	_dot_product_ps( row3, col2, this->v3[Z] );
	_dot_product_ps( row3, col3, this->v3[W] );

	return *this;
}

const Matrix Matrix::operator+( ) const
{
	return *this;
}

const Matrix Matrix::operator-( ) const
{
	return *this * -1.0f;
}

const float Matrix::det() const
{

#ifdef MATH_SIMD
	M128_TYPE a = this->v0.m;
	M128_TYPE b = _mm_setr_ps(v1[Y], v1[X], v1[X], v1[X]);
	M128_TYPE c = _mm_setr_ps(v2[Z], v2[Z], v2[Y], v2[Y]);
	M128_TYPE d = _mm_setr_ps(v3[W], v3[W], v3[W], v3[Z]);
	M128_TYPE e = _mm_setr_ps(v2[W], v2[W], v2[W], v2[Z]);
	M128_TYPE f = _mm_setr_ps(v3[Z], v3[Z], v3[Y], v3[Y]);
	M128_TYPE g = _mm_setr_ps(v1[Z], v1[Z], v1[Y], v1[Y]);
	M128_TYPE h = _mm_setr_ps(v2[Y], v2[X], v2[X], v2[X]);
	M128_TYPE i = d;
	M128_TYPE j = e;
	M128_TYPE k = _mm_setr_ps(v3[Y], v3[X], v3[X], v3[X]);
	M128_TYPE l = _mm_setr_ps(v1[W], v1[W], v1[W], v1[Z]);
	M128_TYPE m = h;
	M128_TYPE n = f;
	M128_TYPE o = c;
	M128_TYPE p = k;

	const static M128_TYPE DETERMINANT_COEFF = _mm_setr_ps(1.0f, -1.0f, 1.0f, -1.0f);

#else // MATH_NO_SIMD
	M128_TYPE a = this->v0.m;
	M128_TYPE b( v1[Y], v1[X], v1[X], v1[X] );
	M128_TYPE c( v2[Z], v2[Z], v2[Y], v2[Y] );
	M128_TYPE d( v3[W], v3[W], v3[W], v3[Z] );
	M128_TYPE e( v2[W], v2[W], v2[W], v2[Z] );
	M128_TYPE f( v3[Z], v3[Z], v3[Y], v3[Y] );
	M128_TYPE g( v1[Z], v1[Z], v1[Y], v1[Y] );
	M128_TYPE h( v2[Y], v2[X], v2[X], v2[X] );
	M128_TYPE i = d;
	M128_TYPE j = e;
	M128_TYPE k( v3[Y], v3[X], v3[X], v3[X] );
	M128_TYPE l( v1[W], v1[W], v1[W], v1[Z] );
	M128_TYPE m = h;
	M128_TYPE n = f;
	M128_TYPE o = c;
	M128_TYPE p = k;

	const static M128_TYPE DETERMINANT_COEFF( 1.0f, -1.0f, 1.0f, -1.0f );
#endif

	M128_TYPE result = a*( b*( c*d - e*f ) - g*( h*i - j*k ) + l*( m*n - o*p ) );
	float det;
	_dot_product_ps( result, DETERMINANT_COEFF, det );

	return det;
}

Matrix& Matrix::T()
{

#ifdef MATH_SIMD
	M128_TYPE col0 = _mm_setr_ps(v0[X], v1[X], v2[X], v3[X]);
	M128_TYPE col1 = _mm_setr_ps(v0[Y], v1[Y], v2[Y], v3[Y]);
	M128_TYPE col2 = _mm_setr_ps(v0[Z], v1[Z], v2[Z], v3[Z]);
	M128_TYPE col3 = _mm_setr_ps(v0[W], v1[W], v2[W], v3[W]);

#else
	M128_TYPE col0( v0[X], v1[X], v2[X], v3[X] );
	M128_TYPE col1( v0[Y], v1[Y], v2[Y], v3[Y] );
	M128_TYPE col2( v0[Z], v1[Z], v2[Z], v3[Z] );
	M128_TYPE col3( v0[W], v1[W], v2[W], v3[W] );

#endif

	this->v0.m = col0;
	this->v1.m = col1;
	this->v2.m = col2;
	this->v3.m = col3;

	return *this;
}

const Matrix Matrix::getT() const
{
	return Matrix( *this ).T();
}

Matrix& Matrix::inv()
{
	float determinant = this->det();

	float b0 = v1[Z] * v2[W] * v3[Y] - v1[W] * v2[Z] * v3[Y] + v1[W] * v2[Y] * v3[Z] - v1[Y] * v2[W] * v3[Z] - v1[Z] * v2[Y] * v3[W] + v1[Y] * v2[Z] * v3[W];
	float b1 = v0[W] * v2[Z] * v3[Y] - v0[Z] * v2[W] * v3[Y] - v0[W] * v2[Y] * v3[Z] + v0[Y] * v2[W] * v3[Z] + v0[Z] * v2[Y] * v3[W] - v0[Y] * v2[Z] * v3[W];
	float b2 = v0[Z] * v1[W] * v3[Y] - v0[W] * v1[Z] * v3[Y] + v0[W] * v1[Y] * v3[Z] - v0[Y] * v1[W] * v3[Z] - v0[Z] * v1[Y] * v3[W] + v0[Y] * v1[Z] * v3[W];
	float b3 = v0[W] * v1[Z] * v2[Y] - v0[Z] * v1[W] * v2[Y] - v0[W] * v1[Y] * v2[Z] + v0[Y] * v1[W] * v2[Z] + v0[Z] * v1[Y] * v2[W] - v0[Y] * v1[Z] * v2[W];
	float b4 = v1[W] * v2[Z] * v3[X] - v1[Z] * v2[W] * v3[X] - v1[W] * v2[X] * v3[Z] + v1[X] * v2[W] * v3[Z] + v1[Z] * v2[X] * v3[W] - v1[X] * v2[Z] * v3[W];
	float b5 = v0[Z] * v2[W] * v3[X] - v0[W] * v2[Z] * v3[X] + v0[W] * v2[X] * v3[Z] - v0[X] * v2[W] * v3[Z] - v0[Z] * v2[X] * v3[W] + v0[X] * v2[Z] * v3[W];
	float b6 = v0[W] * v1[Z] * v3[X] - v0[Z] * v1[W] * v3[X] - v0[W] * v1[X] * v3[Z] + v0[X] * v1[W] * v3[Z] + v0[Z] * v1[X] * v3[W] - v0[X] * v1[Z] * v3[W];
	float b7 = v0[Z] * v1[W] * v2[X] - v0[W] * v1[Z] * v2[X] + v0[W] * v1[X] * v2[Z] - v0[X] * v1[W] * v2[Z] - v0[Z] * v1[X] * v2[W] + v0[X] * v1[Z] * v2[W];
	float b8 = v1[Y] * v2[W] * v3[X] - v1[W] * v2[Y] * v3[X] + v1[W] * v2[X] * v3[Y] - v1[X] * v2[W] * v3[Y] - v1[Y] * v2[X] * v3[W] + v1[X] * v2[Y] * v3[W];
	float b9 = v0[W] * v2[Y] * v3[X] - v0[Y] * v2[W] * v3[X] - v0[W] * v2[X] * v3[Y] + v0[X] * v2[W] * v3[Y] + v0[Y] * v2[X] * v3[W] - v0[X] * v2[Y] * v3[W];
	float b10 = v0[Y] * v1[W] * v3[X] - v0[W] * v1[Y] * v3[X] + v0[W] * v1[X] * v3[Y] - v0[X] * v1[W] * v3[Y] - v0[Y] * v1[X] * v3[W] + v0[X] * v1[Y] * v3[W];
	float b11 = v0[W] * v1[Y] * v2[X] - v0[Y] * v1[W] * v2[X] - v0[W] * v1[X] * v2[Y] + v0[X] * v1[W] * v2[Y] + v0[Y] * v1[X] * v2[W] - v0[X] * v1[Y] * v2[W];
	float b12 = v1[Z] * v2[Y] * v3[X] - v1[Y] * v2[Z] * v3[X] - v1[Z] * v2[X] * v3[Y] + v1[X] * v2[Z] * v3[Y] + v1[Y] * v2[X] * v3[Z] - v1[X] * v2[Y] * v3[Z];
	float b13 = v0[Y] * v2[Z] * v3[X] - v0[Z] * v2[Y] * v3[X] + v0[Z] * v2[X] * v3[Y] - v0[X] * v2[Z] * v3[Y] - v0[Y] * v2[X] * v3[Z] + v0[X] * v2[Y] * v3[Z];
	float b14 = v0[Z] * v1[Y] * v3[X] - v0[Y] * v1[Z] * v3[X] - v0[Z] * v1[X] * v3[Y] + v0[X] * v1[Z] * v3[Y] + v0[Y] * v1[X] * v3[Z] - v0[X] * v1[Y] * v3[Z];
	float b15 = v0[Y] * v1[Z] * v2[X] - v0[Z] * v1[Y] * v2[X] + v0[Z] * v1[X] * v2[Y] - v0[X] * v1[Z] * v2[Y] - v0[Y] * v1[X] * v2[Z] + v0[X] * v1[Y] * v2[Z];

	this->v0[X] = b0;
	this->v0[Y] = b1;
	this->v0[Z] = b2;
	this->v0[W] = b3;
	this->v1[X] = b4;
	this->v1[Y] = b5;
	this->v1[Z] = b6;
	this->v1[W] = b7;
	this->v2[X] = b8;
	this->v2[Y] = b9;
	this->v2[Z] = b10;
	this->v2[W] = b11;
	this->v3[X] = b12;
	this->v3[Y] = b13;
	this->v3[Z] = b14;
	this->v3[W] = b15;

	( *this ) *= ( 1 / determinant );

	return *this;
}

const Matrix Matrix::getInv() const
{
	return Matrix( *this ).inv();
}