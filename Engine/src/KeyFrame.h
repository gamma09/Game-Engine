#pragma once

#include <cstdint>
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
	KeyFrame(const uint32_t& boneCount, const unsigned char* rawFrameData);
	KeyFrame(KeyFrame&& frame);
	KeyFrame& operator=(KeyFrame&& frame);
	~KeyFrame();

	const uint32_t Get_Frame_Time() const;
	const Transform* const Get_Transform(const uint32_t& boneIndex) const;


private:
	KeyFrame(const KeyFrame& frame);
	KeyFrame& operator=(const KeyFrame& frame);
	

private:
	uint32_t frameTimeMillis;
	Transform* boneTransformationData;
};