#include "Animation.h"
#include "DebugPrint.h"
#include "VectApp.h"
#include "QuatApp.h"

Animation::Animation( const char* name ) :
	frames(),
	name( name ),
	framesDropped( 0 )
{
	// Do nothing
}

Animation::Animation( const Animation& anim ) :
	frames( anim.frames ),
	name( anim.name ),
	framesDropped( anim.framesDropped )
{
	// Do nothing
}

Animation& Animation::operator=( const Animation& anim )
{
	this->frames = anim.frames;
	this->name = anim.name;
	this->framesDropped = anim.framesDropped;

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

unsigned int Animation::Get_Number_Of_Frames_Dropped() const
{
	return this->framesDropped;
}

// TODO change these to a model converter knob
#define COMPRESS_QUATERNION_ANGLE_TOLERANCE MATH_PI_180 * 10.0f
#define COMPRESS_TRANSLATION_TOLERANCE 0.1f * 0.1f
#define COMPRESS_SCALE_TOLERANCE 0.01f * 0.01f

static bool Can_Drop_Frame( const Frame& firstFrame, const Frame& frameToDrop, const Frame& lastFrame )
{
	const float t = (float) ( frameToDrop.frameTime - firstFrame.frameTime ) / (float) ( lastFrame.frameTime - firstFrame.frameTime );

	for( unsigned int i = 0; i < firstFrame.bones.size(); i++ )
	{
		const BoneFrame& boneA = firstFrame.bones[i];
		const BoneFrame& boneB = frameToDrop.bones[i];
		const BoneFrame& boneC = lastFrame.bones[i];
		Vect deltaTrans = VectApp::Lerp( boneA.translation, boneC.translation, t ) - boneB.translation;
		Vect deltaScale = VectApp::Lerp( boneA.scale, boneC.scale, t ) - boneB.scale;
		float deltaQuat = QuatApp::Slerp( boneA.rotation, boneC.rotation, t ).getAngleTo( boneB.rotation );

		if( deltaTrans.magSqr() > COMPRESS_TRANSLATION_TOLERANCE ||
			deltaScale.magSqr() > COMPRESS_SCALE_TOLERANCE ||
			deltaQuat > COMPRESS_QUATERNION_ANGLE_TOLERANCE )
		{
			return false;
		}
	}

	return true;
}

void Animation::Compress()
{
	for( unsigned int firstFrame = 0; firstFrame < this->frames.size() - 2; firstFrame++ )
	{
		unsigned int lastFrame = firstFrame + 1;
		while( lastFrame + 1 < this->frames.size() && Can_Drop_Frame( this->frames[firstFrame], this->frames[lastFrame], this->frames[lastFrame + 1] ) )
		{
			lastFrame++;
		}

		if( lastFrame > firstFrame + 1 )
		{
			this->frames.erase( this->frames.begin() + firstFrame + 1, this->frames.begin() + lastFrame );
			framesDropped += lastFrame - firstFrame - 1;
		}
	}
}
