#pragma once

#include <stdint.h>
#include <Vect.h>
#include <Quat.h>

struct Transform
{
	Vect scale;
	Quat rotation;
	Vect translation;
};

class KeyFrame
{
public:
	KeyFrame();
	KeyFrame( uint32_t boneCount, unsigned char*& rawFrameData );
	KeyFrame( const KeyFrame& frame );
	KeyFrame& operator=( const KeyFrame& frame );
	~KeyFrame();

	uint32_t Get_Frame_Time() const;
	const Transform* const Get_Transform( uint32_t boneIndex ) const;


private:


private:
	uint32_t frameTimeMillis;
	Transform* boneTransformationData;
	uint32_t boneCount;
};