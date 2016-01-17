#pragma once

#include <vector>
#include <MathEngine.h>

using namespace std;

struct BoneFrame
{
	Vect scale;
	Quat rotation;
	Vect translation;
};

struct Frame
{
	unsigned int frameTime;

	vector<BoneFrame> bones;
};

int Compare_Frames( const void* frameA, const void* frameB );