#pragma once

#include <string>
#include <fbxsdk.h>
#include <MathEngine.h>

using namespace std;

struct Bone
{
	int parentBone;
	string name;
	Matrix invBindMatrix;
};