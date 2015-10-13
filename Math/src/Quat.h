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
	Quat(const Quat& q);
	Quat& operator=(const Quat& q);
	~Quat();

	Quat(const MatrixSpecialType type);
	Quat(const M128_TYPE& m);
	Quat(const Matrix& mtx);
	Quat(const float& x, const float& y, const float& z, const float& w);
	Quat(const Vect& v);
	Quat(const Vect& v, const float& w);
	Quat(const RotType& type, const float& angle);
	Quat(const Rot3AxisType&, const float& xAngle, const float& yAngle, const float& zAngle);
	Quat(const RotAxisAngleType&, const Vect& axis, const float& angle);
	Quat(const RotOrientType& type, const Vect& direction, const Vect& up);

	const float operator[](const x_enum&) const;
	const float operator[](const y_enum&) const;
	const float operator[](const z_enum&) const;
	const float operator[](const w_enum&) const;

	float& operator[](const x_enum&);
	float& operator[](const y_enum&);
	float& operator[](const z_enum&);
	float& operator[](const w_enum&);

	void set(const MatrixSpecialType type);
	void set(const M128_TYPE& m);
	void set(const Matrix& mtx);
	void set(const Vect& v);
	void set(const Vect& v, const float& qw);
	void set(const float& x, const float& y, const float& z, const float& w);
	void set(const RotType& type, const float& angle);
	void set(const Rot3AxisType&, const float& xAngle, const float& yAngle, const float& zAngle);
	void set(const RotAxisAngleType&, const Vect& axis, const float& angle);
	void set(const RotOrientType& type, const Vect& direction, const Vect& up);

	void setVect(const Vect& v);
	void getVect(Vect& out) const;
	void getAxis(Vect& out) const; // same as getVect?

	const float getAngle() const;

	const Quat multByElement(const Quat& q) const;

	const Quat operator+(const Quat& q) const;
	const Quat operator+(const float& f) const;
	Quat& operator+=(const Quat& q);
	Quat& operator+=(const float& f);

	const Quat operator-(const Quat& q) const;
	const Quat operator-(const float& f) const;
	Quat& operator-=(const Quat& q);
	Quat& operator-=(const float& f);

	const Quat operator*(const Quat& q) const;
	const Quat operator*(const float& f) const;
	Quat& operator*=(const Quat& q);
	Quat& operator*=(const float& f);

	const Quat operator/(const Quat& q) const;
	const Quat operator/(const float& f) const;
	Quat& operator/=(const Quat& q);
	Quat& operator/=(const float& f);
	

	const Quat operator+() const;
	const Quat operator-() const;


	const bool isEqual(const Quat& q, const float& tolerance) const;
	const bool isNegEqual(const Quat& q, const float& tolerance) const;
	const bool isEquivalent(const Quat& q, const float& tolerance) const;
	const bool isConjugateEqual(const Quat& q, const float& tolerance) const;
	const bool isIdentity(const float& tolerance) const;
	const bool isZero(const float& tolerance) const;
	const bool isNormalized(const float& tolerance) const;


	const Quat getConj() const;
	Quat& conj();

	const Quat getT() const;
	Quat& T();

	const float mag() const;
	const float magSquared() const;
	const float invMag() const;

	const Quat getNorm() const;
	Quat& norm();

	const Quat getInv() const;
	Quat& inv();

	const float dot(const Quat& q) const;

	void Lqcvq(const Vect& vIn, Vect& vOut) const;
	void Lqvqc(const Vect& vIn, Vect& vOut) const;

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
	friend const Quat operator/(const float& f, const Quat& q);
};


const Quat operator+(const float& f, const Quat& q);
const Quat operator-(const float& f, const Quat& q);
const Quat operator*(const float& f, const Quat& q);
const Quat operator/(const float& f, const Quat& q);

/**** END of Quat.h ********************************************************/
