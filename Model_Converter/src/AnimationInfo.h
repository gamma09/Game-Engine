#pragma once

#include <vector>

#include "Animation.h"
#include "Bone.h"

class AnimationInfo
{
public:

	AnimationInfo();
	AnimationInfo( const AnimationInfo& info );
	AnimationInfo& operator=( const AnimationInfo& info );
	~AnimationInfo();

	Animation& Add_Animation( const char* name );
	void Add_Bone( const char* boneName, const char* parentName, FbxNode* boneNode );

	const vector<Animation>& Get_Animations() const;
	const vector<Bone>& Get_Hierarchy() const;

private:
	vector<Animation> anims;
	vector<Bone> hierarchy;
};