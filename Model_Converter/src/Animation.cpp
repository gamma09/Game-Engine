#include "Animation.h"


Animation::Animation( const char* name ) :
	frames(),
	name( name )
{
	// Do nothing
}

Animation::Animation( const Animation& anim ) :
	frames( anim.frames ),
	name( anim.name )
{
	// Do nothing
}

Animation& Animation::operator=( const Animation& anim )
{
	this->frames = anim.frames;
	this->name = anim.name;

	return *this;
}

Animation::~Animation()
{
	// Do nothing
}

void Animation::Add_Frame( const Frame& frame )
{
	this->frames.push_back( frame );
}

const char* Animation::Get_Name() const
{
	return this->name.c_str();
}

const vector<Frame>& Animation::Get_Animation_Frames()
{
	qsort( &this->frames[0], this->frames.size(), sizeof( Frame ), Compare_Frames );

	return this->frames;
}