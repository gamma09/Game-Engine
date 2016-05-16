#pragma once

#include <vector>
#include <MathEngine.h>

using namespace std;

struct BoneFrame
{
	Quat rotation;
	Vect translation;
	Vect scale;
};

struct Frame
{
	unsigned int frameTime;

	vector<BoneFrame> bones;
};

int Compare_Frames( const void* frameA, const void* frameB );