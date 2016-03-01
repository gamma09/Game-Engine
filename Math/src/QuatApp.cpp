/*****************************************************************************/
/*                                                                           */
/* File: QuatApp.cpp                                                         */
/*                                                                           */
/* Quaternioin Application Class                                             */
/*                                                                           */
/*****************************************************************************/

#include "MathEngine.h"
#include "QuatApp.h"
#include <math.h>
#include <GameAssert.h>

void QuatApp::Slerp( Quat& result, const Quat& source, const Quat& target, const float t )
{
	float tmp = source.dot( target );
	Quat tar;
	if( tmp < 0 )
	{
		tmp = -tmp;
		tar = -target;
	}
	else
	{
		tar = target;
	}

	if( tmp > 1.0f )
	{
		GameAssert( tmp - ENGINE_MATH_TOLERANCE <= 1.0f );
		tmp = 1.0f;
	}

	const float angle = acosf( tmp );
	if( angle == 0.0f )
	{
		result = source;
	}
	else
	{
		const float sn = sinf( angle );
		result = source * sinf( ( 1 - t ) * angle ) / sn + tar * sinf( t * angle ) / sn;
	}
}

void QuatApp::SlerpArray( Quat* result, const Quat* source, const Quat* target, const float t, const int numQuats )
{
	Quat* endRes = result + numQuats;

	Quat* resPtr = result;
	const Quat* srcPtr = source;
	const Quat* tarPtr = target;

	while( resPtr < endRes )
	{
		Slerp( *resPtr, *srcPtr, *tarPtr, t );

		resPtr++;
		srcPtr++;
		tarPtr++;
	}
}


/***** END of File QuatApp.cpp ***********************************************/
