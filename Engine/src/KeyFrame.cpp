#include <string.h>
#include <GameAssert.h>

#include "KeyFrame.h"
#include "MemorySetup.h"

KeyFrame::KeyFrame()
	: frameTimeMillis( 0 ),
	boneTransformationData( nullptr ),
	boneCount( 0 )
{
	// Do nothing
}

KeyFrame::KeyFrame( uint32_t boneCount, const unsigned char* rawFrameData )
	: boneTransformationData( newArray( Transform, boneCount, AnimHeap::Instance(), ALIGN_16 ) ),
	frameTimeMillis( *reinterpret_cast<const uint32_t*>( rawFrameData ) ),
	boneCount( boneCount )
{
	memcpy( this->boneTransformationData, rawFrameData + sizeof( uint32_t ) / sizeof( unsigned char ), sizeof( Transform ) * boneCount );
}

KeyFrame::KeyFrame( const KeyFrame& frame )
	: frameTimeMillis( frame.frameTimeMillis ),
	boneTransformationData( newArray( Transform, frame.boneCount, AnimHeap::Instance(), ALIGN_16 ) ),
	boneCount( frame.boneCount )
{
	memcpy( this->boneTransformationData, frame.boneTransformationData, sizeof( Transform ) * frame.boneCount );
}

KeyFrame& KeyFrame::operator=( const KeyFrame& frame )
{
	this->frameTimeMillis = frame.frameTimeMillis;
	this->boneCount = frame.boneCount;
	this->boneTransformationData = newArray( Transform, frame.boneCount, AnimHeap::Instance(), ALIGN_16 );
	memcpy( this->boneTransformationData, frame.boneTransformationData, sizeof( Transform ) * frame.boneCount );

	return *this;
}

KeyFrame::~KeyFrame()
{
	if( this->boneTransformationData != 0 )
	{
		delete this->boneTransformationData;
	}
}

uint32_t KeyFrame::Get_Frame_Time() const
{
	GameAssert( this->boneTransformationData != 0 );

	return this->frameTimeMillis;
}

const Transform* const KeyFrame::Get_Transform( uint32_t boneIndex ) const
{
	GameAssert( this->boneTransformationData != 0 );
	GameAssert( boneIndex < this->boneCount );

	return this->boneTransformationData + boneIndex;
}