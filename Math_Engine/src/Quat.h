#pragma once

/** @file */
/*****************************************************************************/
/*                                                                           */
/* file: Quat.h                                                              */
/*                                                                           */
/*****************************************************************************/


/*****************************************************************************/
/* Includes:                                                                 */
/*****************************************************************************/
#include "MathEngineDefs.h"
#include "Matrix.h"
#include "Enum.h"


/*************************************************************************
*
* This class contains functions and data structure for a 4-dimensional
* Quaternions.  The quaternions are internally stored interms of the
* imaginary components (qx,qy,qz) and the real components (qw).
*
* In the Math library, quaternions can be thought of as "rotational"
* matrices, everything that you can represent with rotational matrices
* you can use with quaternions.  Such as quaternion concatenation,
* inverses, transpose, normalize, multiplying with matrices, and with
* vectors.
*
*
**************************************************************************/

class Quat
{
public:
	Quat();
	Quat( const Quat& q );
	Quat& operator=( const Quat& q );
	~Quat();

	Quat( MatrixSpecialType type );
	Quat( const M128_TYPE& m );
	Quat( const Matrix& mtx );
	Quat( float x, float y, float z, float w );
	Quat( const Vect& v );
	Quat( const Vect& v, float w );
	Quat( RotType type, float angle );
	Quat( Rot3AxisType, float xAngle, float yAngle, float zAngle );
	Quat( RotAxisAngleType, const Vect& axis, float angle );
	Quat( RotOrientType type, const Vect& direction, const Vect& up );

	inline float operator[]( x_enum ) const { return this->qx; }
	inline float operator[]( y_enum ) const { return this->qy; }
	inline float operator[]( z_enum ) const { return this->qz; }
	inline float operator[]( w_enum ) const { return this->qw; }

	inline float& operator[]( x_enum ) { return this->qx; }
	inline float& operator[]( y_enum ) { return this->qy; }
	inline float& operator[]( z_enum ) { return this->qz; }
	inline float& operator[]( w_enum ) { return this->qw; }

	void set( MatrixSpecialType type );
	void set( const M128_TYPE& m );
	void set( const Matrix& mtx );
	void set( const Vect& v );
	void set( const Vect& v, float qw );
	void set( float x, float y, float z, float w );
	void set( RotType type, float angle );
	void set( Rot3AxisType, float xAngle, float yAngle, float zAngle );
	void set( RotAxisAngleType, const Vect& axis, float angle );
	void set( RotOrientType type, const Vect& direction, const Vect& up );

	void setVect( const Vect& v );
	void getVect( Vect& out ) const;
	void getAxis( Vect& out ) const; // same as getVect?

	float getAngle() const;
	float getAngleTo( const Quat& b ) const;

	Quat multByElement( const Quat& q ) const;

	Quat operator+( const Quat& q ) const;
	Quat operator+( float f ) const;
	Quat& operator+=( const Quat& q );
	Quat& operator+=( float f );

	Quat operator-( const Quat& q ) const;
	Quat operator-( float f ) const;
	Quat& operator-=( const Quat& q );
	Quat& operator-=( float f );

	Quat operator*( const Quat& q ) const;
	Quat operator*( float f ) const;
	Quat& operator*=( const Quat& q );
	Quat& operator*=( float f );

	Quat operator/( const Quat& q ) const;
	Quat operator/( float f ) const;
	Quat& operator/=( const Quat& q );
	Quat& operator/=( float f );


	Quat operator+( ) const;
	Quat operator-( ) const;


	bool isEqual( const Quat& q, float tolerance ) const;
	bool isNegEqual( const Quat& q, float tolerance ) const;
	bool isEquivalent( const Quat& q, float tolerance ) const;
	bool isConjugateEqual( const Quat& q, float tolerance ) const;
	bool isIdentity( float tolerance ) const;
	bool isZero( float tolerance ) const;
	bool isNormalized( float tolerance ) const;


	Quat getConj() const;
	Quat& conj();

	Quat getT() const;
	Quat& T();

	float mag() const;
	float magSquared() const;
	float invMag() const;

	Quat getNorm() const;
	Quat& norm();

	Quat getInv() const;
	Quat& inv();

	float dot( const Quat& q ) const;

	void Lqcvq( const Vect& vIn, Vect& vOut ) const;
	void Lqvqc( const Vect& vIn, Vect& vOut ) const;

private:

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
			float qx;
			float qy;
			float qz;
			float qw;
		};
	};

	friend class Matrix;
	friend Quat operator/( float f, const Quat& q );
};


Quat operator+( float f, const Quat& q );
Quat operator-( float f, const Quat& q );
Quat operator*( float f, const Quat& q );
Quat operator/( float f, const Quat& q );

/**** END of Quat.h ********************************************************/
