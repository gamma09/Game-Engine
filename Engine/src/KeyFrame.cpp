#include <cstring>

#include "KeyFrame.h"

KeyFrame::KeyFrame() :
	frameTimeMillis(0),
	boneTransformationData(0)
{
	// Do nothing
}

KeyFrame::KeyFrame(const uint32_t& boneCount, const unsigned char* rawFrameData) :
	boneTransformationData(new Transform[boneCount])
{
	this->frameTimeMillis = *reinterpret_cast<const uint32_t*>(rawFrameData);
	memcpy(this->boneTransformationData, rawFrameData + sizeof(uint32_t) / sizeof(unsigned char), sizeof(Transform) * boneCount);
}

KeyFrame::KeyFrame(KeyFrame&& frame) :
	frameTimeMillis(frame.frameTimeMillis),
	boneTransformationData(frame.boneTransformationData)
{
	frame.boneTransformationData = 0;
}

KeyFrame& KeyFrame::operator=(KeyFrame&& frame)
{
	this->frameTimeMillis = frame.frameTimeMillis;
	this->boneTransformationData = frame.boneTransformationData;
	frame.boneTransformationData = 0;

	return *this;
}

KeyFrame::~KeyFrame()
{
	if (this->boneTransformationData != 0)
		delete[] this->boneTransformationData;
}

const uint32_t KeyFrame::Get_Frame_Time() const
{
	assert(this->boneTransformationData != 0);

	return this->frameTimeMillis;
}

const Transform* const KeyFrame::Get_Transform(const uint32_t& boneIndex) const
{
	assert(this->boneTransformationData != 0);

	return this->boneTransformationData + boneIndex;
}