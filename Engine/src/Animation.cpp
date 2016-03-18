#include <utility>

#include <VectApp.h>
#include <QuatApp.h>
#include <GameAssert.h>

#include "Animation.h"
#include "KeyFrame.h"
#include "MemorySetup.h"

Animation::Animation()
	: keyFrameCount( 0 ),
	keyFrames( nullptr )
{
	// Do nothing
}

Animation::Animation( uint32_t boneCount, const unsigned char* frameData )
	: keyFrameCount( *reinterpret_cast<const uint32_t*>( frameData ) )
{
	this->keyFrames = newArray( KeyFrame, this->keyFrameCount, AnimHeap::Instance(), ALIGN_4 );

	const unsigned char* framePtr = frameData + sizeof( uint32_t ) / sizeof( unsigned char );
	const uint32_t frameSize = ( sizeof( uint32_t ) + boneCount * sizeof( Transform ) ) / sizeof( unsigned char );

	for( uint32_t i = 0; i < this->keyFrameCount; i++, framePtr += frameSize )
	{
		this->keyFrames[i] = KeyFrame( boneCount, framePtr );
	}
}

Animation::Animation( const Animation& anim )
	: keyFrameCount( anim.keyFrameCount ),
	keyFrames( newArray( KeyFrame, anim.keyFrameCount, AnimHeap::Instance(), ALIGN_4 ) )
{
	for( unsigned int i = 0; i < anim.keyFrameCount; i++ )
	{
		this->keyFrames[i] = anim.keyFrames[i];
	}
}

Animation& Animation::operator=( const Animation& anim )
{
	this->keyFrameCount = anim.keyFrameCount;
	this->keyFrames = newArray( KeyFrame, anim.keyFrameCount, AnimHeap::Instance(), ALIGN_4 );
	for( unsigned int i = 0; i < anim.keyFrameCount; i++ )
	{
		this->keyFrames[i] = anim.keyFrames[i];
	}

	return *this;
}

Animation::~Animation()
{
	if( this->keyFrames != nullptr )
		delete this->keyFrames;
}

Matrix Animation::Get_Transform( uint32_t time, uint32_t boneIndex ) const
{
	GameAssert( this->keyFrameCount > 0 );
	GameAssert( this->keyFrames != 0 );
	GameAssert( time <= this->keyFrames[this->keyFrameCount - 1].Get_Frame_Time() );

	KeyFrame* prevFramePtr = 0;
	KeyFrame* nextFramePtr = this->keyFrames;

	uint32_t prevFrameTime = 0;
	uint32_t nextFrameTime = nextFramePtr->Get_Frame_Time();

	while( nextFrameTime < time )
	{
		prevFramePtr = nextFramePtr++;

		prevFrameTime = nextFrameTime;
		nextFrameTime = nextFramePtr->Get_Frame_Time();
	}

	if( nextFrameTime > time )
	{
		const float factor = ( (float) ( time - prevFrameTime ) ) / ( (float) ( nextFrameTime - prevFrameTime ) );

		const Transform* const prev = prevFramePtr->Get_Transform( boneIndex );
		const Transform* const next = nextFramePtr->Get_Transform( boneIndex );

		Matrix result( SCALE, VectApp::Lerp( prev->scale, next->scale, factor ) );

		Quat rotation;
		QuatApp::Slerp( rotation, prev->rotation, next->rotation, factor );
		result *= rotation;
		result *= Matrix( TRANS, VectApp::Lerp( prev->translation, next->translation, factor ) );
		return result;
	}
	else // nextFrameTime == time
	{
		const Transform* const transform = nextFramePtr->Get_Transform( boneIndex );
		Matrix result( SCALE, transform->scale );
		result *= transform->rotation;
		result *= Matrix( TRANS, transform->translation );
		return result;
	}
}

uint32_t Animation::Get_KeyFrame_Count() const
{
	return this->keyFrameCount;
}

uint32_t Animation::Get_Animation_Length() const
{
	GameAssert( this->keyFrameCount > 0 );
	GameAssert( this->keyFrames != 0 );

	return this->keyFrames[this->keyFrameCount - 1].Get_Frame_Time();
}