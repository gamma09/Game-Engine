#pragma once

#include <cstdint>
#include <Matrix.h>

class KeyFrame;

class Animation
{
public:

	Animation();
	Animation( uint32_t boneCount, const unsigned char* frameData );
	Animation( Animation&& anim );
	Animation& operator=( Animation&& anim );
	~Animation();

	Matrix Get_Transform( uint32_t time, uint32_t boneIndex ) const;
	uint32_t Get_KeyFrame_Count() const;
	uint32_t Get_Animation_Length() const;

private:
	Animation( const Animation& anim );
	Animation& operator=( const Animation& anim );

private:
	uint32_t keyFrameCount;
	KeyFrame* keyFrames;
};