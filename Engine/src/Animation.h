#pragma once

#include <cstdint>
#include <Matrix.h>

class KeyFrame;

class Animation
{
public:

	Animation();
	Animation(const uint32_t& boneCount, const unsigned char* frameData);
	Animation(Animation&& anim);
	Animation& operator=(Animation&& anim);
	~Animation();

	const Matrix Get_Transform(const uint32_t& time, const uint32_t& boneIndex) const;
	const uint32_t Get_KeyFrame_Count() const;
	const uint32_t Get_Animation_Length() const;

private:
	Animation(const Animation& anim);
	Animation& operator=(const Animation& anim);

private:
	uint32_t keyFrameCount;
	KeyFrame* keyFrames;
};