#ifndef _INTERPOLATION_H_
#define _INTERPOLATION_H_

#define TOLERANCE 0.0001f

inline float4 slerp( float4 source, float4 target, float t )
{
	float tmp = dot( source, target );
	// float tmpSign = sign( tmp );
	// tmp *= tmpSign;
	// quatB *= tmpSign;
	float4 tar;
	if( tmp < 0.0f )
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
#ifdef _DEBUG
		if( tmp - TOLERANCE >= 1.0f )
		{
			errorf( "Quaternions for slerp are not unit length." );
		}
#endif

		tmp = 1.0f;
	}

	float angle = acos( tmp );
	if( angle == 0.0f )
	{
		return source;
	}
	else
	{
		float invSn = rcp( sin( angle ) );
		return source * sin( ( 1.0f - t ) * angle ) * invSn + tar * sin( t * angle ) * invSn;
	}
}

inline float4x4 CreateScaleMatrix( float4 scale )
{
	float4x4 result =
	{
		scale.x, 0.0f, 0.0f, 0.0f,
		0.0f, scale.y, 0.0f, 0.0f,
		0.0f, 0.0f, scale.z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	return result;
}

inline float4x4 CreateRotationMatrixFromQuaternion( float4 quat )
{
	float4x4 result =
	{
		-1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, -1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	float4 a ={ quat.yxx, 0.0f };
	float4 b ={ quat.yyz, 0.0f };
	float4 c ={ quat.zw, -quat.w, 0.0f };
	float4 d ={ quat.zzy, 0.0f };

	float4 e ={ quat.xxy, 0.0f };
	float4 f ={ quat.yxz, 0.0f };
	float4 g ={ -quat.w, quat.zw, 0.0f };
	float4 h ={ quat.zzx, 0.0f };

	float4 i ={ quat.xyx, 0.0f };
	float4 j ={ quat.zzx, 0.0f };
	float4 k ={ quat.w, -quat.w, quat.y, 0.0f };
	float4 l ={ quat.yxy, 0.0f };

	float4x4 result2 =
	{
		2.0f * ( a * b + c * d ),
		2.0f * ( e * f + g * h ),
		2.0f * ( i * j + k * l ),
		0.0f, 0.0f, 0.0f, 0.0f
	};

	result += result2;

	result._11_22_33 *= -1.0f;

#ifdef _DEBUG
	if( abs( result._14 ) > TOLERANCE )
	{
		errorf( "Need to set _14 to 0.0 in slerp!" );
	}

	if( abs( result._24 ) > TOLERANCE )
	{
		errorf( "Need to set _24 to 0.0 in slerp!" );
	}

	if( abs( result._34 ) > TOLERANCE )
	{
		errorf( "Need to set _34 to 0.0 in slerp!" );
	}
#endif

	return result;
}

inline float4x4 CreateTranslationMatrix( float4 trans )
{
	float4x4 result =
	{
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		trans.xyz,        1.0f
	};

	return result;
}

inline float4x4 InterpolateTranslation( float4 transA, float4 transB, float InterpolationTime )
{
	float4 translation;
	translation = lerp( transA, transB, InterpolationTime );
	return CreateTranslationMatrix( translation );
}

inline float4x4 InterpolateRotation( float4 rotationA, float4 rotationB, float InterpolationTime )
{
	float4 rotation;
	rotation = slerp( rotationA, rotationB, InterpolationTime );
	return CreateRotationMatrixFromQuaternion( rotation );
}

inline float4x4 InterpolationScale( float4 scaleA, float4 scaleB, float InterpolationTime )
{
	float4 scale;
	scale = lerp( scaleA, scaleB, InterpolationTime );
	return CreateScaleMatrix( scale );
}

#endif