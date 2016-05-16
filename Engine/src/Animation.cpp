#include <utility>

#include <VectApp.h>
#include <QuatApp.h>
#include <GameAssert.h>
#include <DebuggerSetup.h>

#include "Animation.h"
#include "KeyFrame.h"
#include "MemorySetup.h"
#include "SharedShaderDefines.h"

Animation::Animation()
	: keyFrameCount( 0 ),
	keyFrames( nullptr )
{
	// Do nothing
}

Animation::Animation( ID3D11Device* device, uint32_t boneCount, unsigned char*& frameData )
{
	this->keyFrameCount = *reinterpret_cast<const uint32_t*>( frameData );
	frameData += sizeof( uint32_t );

	this->keyFrames = newArray( KeyFrame, this->keyFrameCount, AnimHeap::Instance(), ALIGN_4 );
	memset( this->keyFrames, 0, sizeof( KeyFrame ) * this->keyFrameCount );

	for( uint32_t i = 0; i < this->keyFrameCount; i++ )
	{
		this->keyFrames[i] = std::move( KeyFrame( device, boneCount, frameData ) );
	}
}

Animation::Animation( Animation&& anim )
	: keyFrameCount( anim.keyFrameCount ),
	keyFrames( anim.keyFrames )
{
	GameAssert( anim.keyFrames );

	anim.keyFrames = nullptr;
}

Animation& Animation::operator=( Animation&& anim )
{
	GameAssert( anim.keyFrames );

	this->keyFrameCount = anim.keyFrameCount;
	this->keyFrames = anim.keyFrames;
	anim.keyFrames = nullptr;

	return *this;
}

Animation::~Animation()
{
	if( this->keyFrames != nullptr )
	{
		for( unsigned int i = 1; i < this->keyFrameCount; i++ )
		{
			this->keyFrames[i].~KeyFrame();
		}

		out( "Deleting keyFrames: %p\n", this->keyFrames );
		delete this->keyFrames;
	}
}

float Animation::Activate_Key_Frame_SRVs( ID3D11DeviceContext* context, uint32_t time ) const
{
	GameAssert( this->keyFrameCount > 1 );
	GameAssert( this->keyFrames );
	GameAssert( time <= this->keyFrames[this->keyFrameCount - 1].Get_Frame_Time() );

	KeyFrame* prevFramePtr = this->keyFrames;
	KeyFrame* nextFramePtr = this->keyFrames + 1;
	
	uint32_t prevFrameTime = 0;
	uint32_t nextFrameTime = nextFramePtr->Get_Frame_Time();
	
	while( nextFrameTime < time )
	{
		prevFramePtr = nextFramePtr++;

		prevFrameTime = nextFrameTime;
		nextFrameTime = nextFramePtr->Get_Frame_Time();
	}

	prevFramePtr->Activate( context, CS_PREV_FRAME_REGISTER );
	nextFramePtr->Activate( context, CS_NEXT_FRAME_REGISTER );

	float timeBetweenFrames = (float) ( nextFrameTime - prevFrameTime );
	float timeSincePrevFrame = (float) ( time - prevFrameTime );

	GameAssert( timeSincePrevFrame <= timeBetweenFrames );

	return timeSincePrevFrame / timeBetweenFrames;
}

uint32_t Animation::Get_Animation_Length() const
{
	GameAssert( this->keyFrameCount > 0 );
	GameAssert( this->keyFrames != 0 );

	return this->keyFrames[this->keyFrameCount - 1].Get_Frame_Time();
}