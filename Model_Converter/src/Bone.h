#pragma once

#include <string>
#include <fbxsdk.h>

struct Bone
{
	int parentBone;
	std::string name;
	FbxNode* node;
};