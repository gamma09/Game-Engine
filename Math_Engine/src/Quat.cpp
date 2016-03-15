/*****************************************************************************/
/*                                                                           */
/* File: Quat.cpp                                                            */
/*                                                                           */
/* Quaterion Class                                                           */
/*                                                                           */
/*****************************************************************************/

#include "MathEngine.h"
#include <math.h>
#include <GameAssert.h>
#include "Quat.h"

#ifdef MATH_SIMD
const static M128_TYPE IDENTITY_QUAT = _mm_setr_ps(0.0f, 0.0f, 0.0f, 1.0f);
const static M128_TYPE ZERO_QUAT = _mm_set1_ps(0.0f);

#else // MATH_NO_SIMD
const static M128_TYPE IDENTITY_QUAT( 0.0f, 0.0f, 0.0f, 1.0f );
const static M128_TYPE ZERO_QUAT( 0.0f );

#endif


//--------------------------------------------------------------------------------------------------
// Contructors
//--------------------------------------------------------------------------------------------------
Quat::Quat()
	: m( IDENTITY_QUAT )
{
	CHECK_ALIGNMENT_16( this );
}

Quat::Quat( const Quat& q )
	: m( q.m )
{
	CHECK_ALIGNMENT_16( this );
}

Quat::Quat( MatrixSpecialType type )
{
	CHECK_ALIGNMENT_16( this );

	switch( type )
	{
		case IDENTITY:
			this->m = IDENTITY_QUAT;
			break;

		case ZERO:
			this->m = ZERO_QUAT;
			break;

		default:
			GameAssert( 0 );
			break;
	}
}

Quat::Quat( const M128_TYPE& _m )
	: m( _m )
{
	CHECK_ALIGNMENT_16( this );
}

Quat::Quat( const Matrix& mtx )
{
	CHECK_ALIGNMENT_16( this );

	this->set( mtx );
}

Quat::Quat( float x, float y, float z, float w )
	: qx( x ),
	qy( y ),
	qz( z ),
	qw( w )
{
	CHECK_ALIGNMENT_16( this );
}

Quat::Quat( const Vect& v )
	: m( v.m )
{
	CHECK_ALIGNMENT_16( this );
}

Quat::Quat( const Vect& v, float w )
	: m( v.m )
{
	CHECK_ALIGNMENT_16( this );

	this->qw = w;
}

Quat::Quat( RotType type, float angle )
{
	CHECK_ALIGNMENT_16( this );

	this->set( type, angle );
}

Quat::Quat( Rot3AxisType, float xAngle, float yAngle, float zAngle )
{
	CHECK_ALIGNMENT_16( this );

	this->set( ROT_XYZ, xAngle, yAngle, zAngle );
}

Quat::Quat( RotAxisAngleType, const Vect& axis, float angle )
{
	CHECK_ALIGNMENT_16( this );

	this->set( ROT_AXIS_ANGLE, axis, angle );
}

Quat::Quat( RotOrientType type, const Vect& direction, const Vect& up )
{
	CHECK_ALIGNMENT_16( this );

	this->set( type, direction, up );
}




//--------------------------------------------------------------------------------------------------
// Set functions
//--------------------------------------------------------------------------------------------------
void Quat::set( MatrixSpecialType type )
{
	switch( type )
	{
		case IDENTITY:
			this->m = IDENTITY_QUAT;
			break;

		case ZERO:
			this->m = ZERO_QUAT;
			break;

		default:
			GameAssert( 0 );
			break;
	}
}

void Quat::set( const M128_TYPE& _m )
{
	this->m = _m;
}

void Quat::set( const Matrix& mtx )
{
	float tr = mtx[m0] + mtx[m5] + mtx[m10];


	// Case 1
	if( tr > 0.0f )
	{
		float S = 2.0f * sqrtf( tr + 1.0f );
		this->qx = -( mtx[m9] - mtx[m6] ) / S;
		this->qy = -( mtx[m2] - mtx[m8] ) / S;
		this->qz = -( mtx[m4] - mtx[m1] ) / S;

#ifdef MATH_SIMD
		M128_TYPE a = _mm_setr_ps(mtx[m9], mtx[m2], mtx[m4], 0.0f);
		M128_TYPE b = _mm_setr_ps(mtx[m6], mtx[m8], mtx[m1], 0.0f);
		M128_TYPE m_S = _mm_set1_ps(-1.0f / S);

#else // MATH_NO_SIMD
		M128_TYPE a( mtx[m9], mtx[m2], mtx[m4], 0.0f );
		M128_TYPE b( mtx[m6], mtx[m8], mtx[m1], 0.0f );
		M128_TYPE m_S( -1.0f / S );
#endif
		this->m = ( a - b ) * m_S;

		this->qw = 0.25f * S;


		// Case 2
	}
	else if( mtx[m0] > mtx[m5] && mtx[m0] > mtx[m10] )
	{
		float S = 2.0f * sqrtf( 1.0f + mtx[m0] - mtx[m5] - mtx[m10] );
		float inv_S = 1.0f / S;

#ifdef MATH_SIMD
		M128_TYPE a = _mm_setr_ps(0.0f, mtx[m1], mtx[m2], mtx[m9]);
		M128_TYPE b = _mm_setr_ps(0.0f, mtx[m4], mtx[m8], -mtx[m6]);
		M128_TYPE m_S = _mm_setr_ps(0.0f, inv_S, inv_S, -inv_S);

#else // MATH_NO_SIMD
		M128_TYPE a( 0.0f, mtx[m1], mtx[m2], mtx[m9] );
		M128_TYPE b( 0.0f, mtx[m4], mtx[m8], -mtx[m6] );
		M128_TYPE m_S( 0.0f, inv_S, inv_S, -inv_S );
#endif

		this->m = ( a + b ) * m_S;
		this->qx = 0.25f * S;


		// Case 3
	}
	else if( mtx[m5] > mtx[m10] )
	{
		float S = 2.0f * sqrtf( 1.0f + mtx[m5] - mtx[m0] - mtx[m10] );
		float inv_S = 1.0f / S;

#ifdef MATH_SIMD
		M128_TYPE a = _mm_setr_ps(mtx[m1], 0.0f, mtx[m6], mtx[m2]);
		M128_TYPE b = _mm_setr_ps(mtx[m4], 0.0f, mtx[m9], -mtx[m8]);
		M128_TYPE m_S = _mm_setr_ps(inv_S, 0.0f, inv_S, -inv_S);

#else // MATH_NO_SIMD
		M128_TYPE a( mtx[m1], 0.0f, mtx[m6], mtx[m2] );
		M128_TYPE b( mtx[m4], 0.0f, mtx[m9], -mtx[m8] );
		M128_TYPE m_S( inv_S, 0.0f, inv_S, -inv_S );
#endif

		this->m = ( a + b ) * m_S;
		this->qy = 0.25f * S;


		// Case 4
	}
	else
	{
		float S = 2.0f * sqrtf( 1.0f + mtx[m10] - mtx[m0] - mtx[m5] );
		float inv_S = 1.0f / S;

#ifdef MATH_SIMD
		M128_TYPE a = _mm_setr_ps(mtx[m2], mtx[m6], 0.0f, mtx[m4]);
		M128_TYPE b = _mm_setr_ps(mtx[m8], mtx[m9], 0.0f, -mtx[m1]);
		M128_TYPE m_S = _mm_setr_ps(inv_S, inv_S, 0.0f, -inv_S);

#else // MATH_NO_SIMD
		M128_TYPE a( mtx[m2], mtx[m6], 0.0f, mtx[m4] );
		M128_TYPE b( mtx[m8], mtx[m9], 0.0f, -mtx[m1] );
		M128_TYPE m_S( inv_S, inv_S, 0.0f, -inv_S );
#endif

		this->m = ( a + b ) * m_S;
		this->qz = 0.25f * S;
	}
}

void Quat::set( const Vect& v )
{
	this->m = v.m;
}



void Quat::set( const Vect& v, float qw )
{
	this->m = v.m;
	this->qw = qw;
}

void Quat::set( float x, float y, float z, float w )
{
	this->qx = x;
	this->qy = y;
	this->qz = z;
	this->qw = w;
}

void Quat::set( RotType type, float angle )
{
	this->m = IDENTITY_QUAT;

	const float sn = sinf( angle / 2.0f );
	const float cs = cosf( angle / 2.0f );

	switch( type )
	{
		case ROT_X:
			this->qx = 1.0f * sn;
			break;

		case ROT_Y:
			this->qy = 1.0f * sn;
			break;

		case ROT_Z:
			this->qz = 1.0f * sn;
			break;

		default:
			GameAssert( 0 );
			break;
	}

	this->qw = cs;
}

void Quat::set( Rot3AxisType, float xAngle, float yAngle, float zAngle )
{
	this->set( ROT_X, xAngle );

	Quat temp( ROT_Y, yAngle );
	*this *= temp;

	temp.set( ROT_Z, zAngle );
	*this *= temp;
}

void Quat::set( RotAxisAngleType, const Vect& axis, float angle )
{
	this->m = axis.getNorm().m;

	const float cs = cosf( angle / 2.0f );
	const float sn = sinf( angle / 2.0f );

	this->m *= sn;

	this->qw = cs;
}

void Quat::set( RotOrientType type, const Vect& direction, const Vect& up )
{
	this->set( Matrix( type, direction, up ) );
}




//--------------------------------------------------------------------------------------------------
// getter and setter for vect
//--------------------------------------------------------------------------------------------------
void Quat::setVect( const Vect& v )
{
	const float temp = this->qw;

	this->m = v.m;
	this->qw = temp;
}

void Quat::getVect( Vect& out ) const
{
	out = Vect( this->m, 1.0f );
}

float Quat::getAngle() const
{
	return 2.0f * acosf( this->qw );
}

void Quat::getAxis( Vect& out ) const
{
	out = Vect( this->m, 1.0f );
}





//--------------------------------------------------------------------------------------------------
// Assignment operator & Destructor
//--------------------------------------------------------------------------------------------------
Quat& Quat::operator=( const Quat& q )
{
	this->m = q.m;
	return *this;
}

Quat::~Quat()
{
	// Do nothing
}






//--------------------------------------------------------------------------------------------------
// Addition operators
//--------------------------------------------------------------------------------------------------
Quat Quat::operator+( const Quat& q ) const
{
	return ( Quat( *this ) += q );
}

Quat& Quat::operator+=( const Quat& q )
{
	this->m += q.m;
	return *this;
}

Quat Quat::operator+( float f ) const
{
	return ( Quat( *this ) += f );
}

Quat& Quat::operator+=( float f )
{
#ifdef MATH_SIMD
	M128_TYPE m_f = _mm_set1_ps(f);

#else // MATH_NO_SIMD
	M128_TYPE m_f( f );

#endif

	this->m += m_f;
	return *this;
}

Quat operator+( float f, const Quat& q )
{
	return q + f;
}




//--------------------------------------------------------------------------------------------------
// Subtraction operators
//--------------------------------------------------------------------------------------------------
Quat Quat::operator-( const Quat& q ) const
{
	return ( Quat( *this ) -= q );
}

Quat Quat::operator-( float f ) const
{
	return ( Quat( *this ) -= f );
}

Quat& Quat::operator-=( const Quat& q )
{
	this->m -= q.m;
	return *this;
}

Quat& Quat::operator-=( float f )
{
#ifdef MATH_SIMD
	M128_TYPE m_f = _mm_set1_ps(f);
#else // MATH_NO_SIMD
	M128_TYPE m_f( f );
#endif

	this->m -= m_f;
	return *this;
}

Quat operator-( float f, const Quat& q )
{
	return -q + f;
}




//--------------------------------------------------------------------------------------------------
// Multiplication operators
//--------------------------------------------------------------------------------------------------
Quat Quat::operator*( const Quat& q ) const
{
	return ( Quat( *this ) *= q );
}

Quat& Quat::operator*=( const Quat& q )
{
	const Vect pv( this->m );
	const Vect qv( q.m );

	const float pw = this->qw;
	const float qw = q.qw;

	this->m = qv.cross( pv ).m;
	this->m += ( qw * pv ).m;
	this->m += ( pw * qv ).m;

	this->qw = pw * qw - pv.dot( qv );

	return *this;
}

Quat Quat::operator*( float f ) const
{
	return ( Quat( *this ) *= f );
}

Quat& Quat::operator*=( float f )
{
#ifdef MATH_SIMD
	M128_TYPE m_f = _mm_set1_ps(f);
#else // MATH_NO_SIMD
	M128_TYPE m_f( f );
#endif

	this->m *= m_f;
	return *this;
}

Quat operator*( float f, const Quat& q )
{
	return q * f;
}

Quat Quat::multByElement( const Quat& q ) const
{
	return Quat( this->m * q.m );
}




//--------------------------------------------------------------------------------------------------
// Division operators
//--------------------------------------------------------------------------------------------------
Quat Quat::operator/( const Quat& q ) const
{
	return ( Quat( *this ) /= q );
}

Quat& Quat::operator/=( const Quat& q )
{
	this->m /= q.m;
	return *this;
}

Quat Quat::operator/( float f ) const
{
	return ( Quat( *this ) /= f );
}

Quat& Quat::operator/=( float f )
{
#ifdef MATH_SIMD
	M128_TYPE m_f = _mm_set1_ps(f);
#else // MATH_NO_SIMD
	M128_TYPE m_f( f );
#endif

	this->m /= m_f;
	return *this;
}

Quat operator/( float f, const Quat& q )
{
#ifdef MATH_SIMD
	M128_TYPE m_f = _mm_set1_ps(f);
#else // MATH_NO_SIMD
	M128_TYPE m_f( f );
#endif

	return Quat( m_f / q.m );
}




//--------------------------------------------------------------------------------------------------
// Unary operators
//--------------------------------------------------------------------------------------------------
Quat Quat::operator+( ) const
{
	return Quat( *this );
}

Quat Quat::operator-( ) const
{
	return Quat( this->m * -1.0f );
}




//--------------------------------------------------------------------------------------------------
// Boolean functions
//--------------------------------------------------------------------------------------------------
bool Quat::isEqual( const Quat& q, float tolerance ) const
{
	M128_TYPE a = _mm_abs_ps( this->m - q.m );
	return a.m128_f32[0] < tolerance &&
		a.m128_f32[1] < tolerance &&
		a.m128_f32[2] < tolerance &&
		a.m128_f32[3] < tolerance;
}

bool Quat::isNegEqual( const Quat& q, float tolerance ) const
{
	M128_TYPE a = _mm_abs_ps( this->m + q.m );
	return a.m128_f32[0] < tolerance &&
		a.m128_f32[1] < tolerance &&
		a.m128_f32[2] < tolerance &&
		a.m128_f32[3] < tolerance;
}

bool Quat::isEquivalent( const Quat& q, float tolerance ) const
{
	return this->isEqual( q, tolerance ) || this->isNegEqual( q, tolerance );
}

bool Quat::isConjugateEqual( const Quat& q, float tolerance ) const
{
#ifdef MATH_SIMD
	static const M128_TYPE wNeg = _mm_setr_ps(1.0f, 1.0f, 1.0f, -1.0f);
#else // MATH_NO_SIMD
	static const M128_TYPE wNeg( 1.0f, 1.0f, 1.0f, -1.0f );
#endif

	M128_TYPE a = _mm_abs_ps( this->m * wNeg + q.m );
	return a.m128_f32[0] < tolerance &&
		a.m128_f32[1] < tolerance &&
		a.m128_f32[2] < tolerance &&
		a.m128_f32[3] < tolerance;
}

bool Quat::isIdentity( float tolerance ) const
{
	M128_TYPE a = _mm_abs_ps( this->m - IDENTITY_QUAT );
	return a.m128_f32[0] < tolerance &&
		a.m128_f32[1] < tolerance &&
		a.m128_f32[2] < tolerance &&
		a.m128_f32[3] < tolerance;
}

bool Quat::isZero( float tolerance ) const
{
	M128_TYPE a = _mm_abs_ps( this->m );
	return a.m128_f32[0] < tolerance &&
		a.m128_f32[1] < tolerance &&
		a.m128_f32[2] < tolerance &&
		a.m128_f32[3] < tolerance;
}

bool Quat::isNormalized( float tolerance ) const
{
	return fabsf( this->magSquared() - 1.0f ) < tolerance;
}





//--------------------------------------------------------------------------------------------------
// Special functions
//--------------------------------------------------------------------------------------------------
#ifdef MATH_SIMD
static const M128_TYPE NEGATE_XYZ = _mm_setr_ps(-1.0f, -1.0f, -1.0f, 1.0f);
#else // MATH_NO_SIMD
static const M128_TYPE NEGATE_XYZ( -1.0f, -1.0f, -1.0f, 1.0f );
#endif

Quat Quat::getConj() const
{
	return Quat( *this ).conj();
}

Quat& Quat::conj()
{
	this->m *= NEGATE_XYZ;
	return *this;
}

Quat Quat::getT() const
{
	return this->getConj();
}

Quat& Quat::T()
{
	return this->conj();
}

float Quat::mag() const
{
	return sqrtf( this->magSquared() );
}

float Quat::magSquared() const
{
	float result;
	_dot_product_ps( this->m, this->m, result );
	return result;
}

float Quat::invMag() const
{
	return 1.0f / this->mag();
}

Quat Quat::getNorm() const
{
	return Quat( *this ).norm();
}

Quat& Quat::norm()
{
	GameAssert( !this->isZero( MATH_TOLERANCE ) );

	float magSqr = this->magSquared();

#ifdef MATH_SIMD
	M128_TYPE rMag = _mm_rsqrt_ps(_mm_set1_ps(magSqr));
#else // MATH_NO_SIMD
	const float recSqrtMag = 1.0f / sqrtf( magSqr );
	M128_TYPE rMag( recSqrtMag );
#endif

	this->m *= rMag;

	return *this;
}


Quat Quat::getInv() const
{
	return Quat( *this ).inv();
}

Quat& Quat::inv()
{
	const float magSqr = this->magSquared();

	return ( this->conj() /= magSqr );
}

float Quat::dot( const Quat& q ) const
{
	float result;
	_dot_product_ps( this->m, q.m, result );
	return result;
}

void Quat::Lqcvq( const Vect& vIn, Vect& vOut ) const
{
	const Vect qv( this->m );
	vOut = 2.0f * this->qw * qv.cross( vIn ) + ( this->qw * this->qw - qv.dot( qv ) ) * vIn + 2.0f * qv.dot( vIn ) * qv;
}

void Quat::Lqvqc( const Vect& vIn, Vect& vOut ) const
{
	const Vect qv( this->m );
	vOut = 2.0f * this->qw * vIn.cross( qv ) + ( this->qw * this->qw - qv.dot( qv ) ) * vIn + 2.0f * qv.dot( vIn ) * qv;
}