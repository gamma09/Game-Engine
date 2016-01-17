#include "AnimationInfo.h"
#include <GameAssert.h>

AnimationInfo::AnimationInfo() :
	anims()
{
	// Do nothing
}

AnimationInfo::AnimationInfo( const AnimationInfo& info ) :
	anims( info.anims )
{
	// Do nothing
}

AnimationInfo& AnimationInfo::operator=( const AnimationInfo& info )
{
	this->anims = info.anims;

	return *this;
}

AnimationInfo::~AnimationInfo()
{
	// Do nothing
}

Animation& AnimationInfo::Add_Animation( const char* name )
{
	this->anims.push_back( Animation( name ) );
	return this->anims.back();
}

void AnimationInfo::Add_Bone( const char* boneName, const char* parentName, FbxNode* boneNode )
{
	Bone bone;
	bone.name = boneName;
	bone.node = boneNode;

	int i = ( parentName == 0 ) ? ( -1 ) : ( this->hierarchy.size() - 1 );
	while( i >= 0 && this->hierarchy[i].name != parentName )
		i--;

	if( i == -1 )
		GameAssert( parentName == 0 );

	bone.parentBone = i;
	this->hierarchy.push_back( bone );
}

const vector<Animation>& AnimationInfo::Get_Animations() const
{
	return this->anims;
}

const vector<Bone>& AnimationInfo::Get_Hierarchy() const
{
	return this->hierarchy;
}