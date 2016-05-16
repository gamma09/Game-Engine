#include <d3d11.h>
#include <string.h>
#include <GameAssert.h>
#include <ReferencedObject.h>

#include "DirectXAssert.h"
#include "KeyFrame.h"
#include "MemorySetup.h"
#include "SharedShaderDefines.h"

struct Transform
{
	Quat rotation;
	Vect translation;
	Vect scale;
};

KeyFrame::KeyFrame()
	: frameTimeMillis( 0 ),
	frameSRV( nullptr ),
	frameData( nullptr )
{
	// Do nothing
}

KeyFrame::KeyFrame( ID3D11Device* device, uint32_t boneCount, unsigned char*& rawFrameData )
{
	this->frameTimeMillis = *reinterpret_cast<const uint32_t*>( rawFrameData );
	rawFrameData += sizeof( uint32_t );

	Transform* boneTransforms = newArray( Transform, MAX_BONES, TemporaryHeap::Instance(), ALIGN_4 );
	memset( boneTransforms, 0, sizeof( Transform ) * MAX_BONES );
	memcpy( boneTransforms, rawFrameData, sizeof( Transform ) * boneCount );
	rawFrameData += sizeof( Transform ) * boneCount;

	D3D11_BUFFER_DESC bd;
	bd.ByteWidth = sizeof( Transform ) * MAX_BONES;
	bd.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bd.StructureByteStride = sizeof( Transform );
	bd.Usage = D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA bdInit;
	bdInit.pSysMem = boneTransforms;
	bdInit.SysMemPitch = 0;
	bdInit.SysMemSlicePitch = 0;

	GameCheckFatalDx( device->CreateBuffer( &bd, &bdInit, &this->frameData ), "Could not create buffer to hold key frame transformation data." );
	delete boneTransforms;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	srvDesc.BufferEx.FirstElement = 0;
	srvDesc.BufferEx.NumElements = MAX_BONES;
	srvDesc.BufferEx.Flags = 0;

	GameCheckFatalDx( device->CreateShaderResourceView( this->frameData, &srvDesc, &this->frameSRV ), "Could not create SRV for keyframe data." );
}

KeyFrame::KeyFrame( KeyFrame&& frame )
	: frameTimeMillis( frame.frameTimeMillis ),
	frameSRV( frame.frameSRV ),
	frameData( frame.frameData )
{
	frame.frameSRV = nullptr;
	frame.frameData = nullptr;
}

KeyFrame& KeyFrame::operator=( KeyFrame&& frame )
{
	GameAssert( !this->frameData );
	GameAssert( !this->frameSRV );

	this->frameTimeMillis = frame.frameTimeMillis;
	this->frameSRV = frame.frameSRV;
	this->frameData = frame.frameData;

	frame.frameSRV = nullptr;
	frame.frameData = nullptr;

	return *this;
}

KeyFrame::~KeyFrame()
{
	if( this->frameSRV )
	{
		this->frameSRV->Release();
	}

	this->frameSRV = nullptr;

	if( this->frameData )
	{
		this->frameData->Release();
	}

	this->frameData = nullptr;
}

uint32_t KeyFrame::Get_Frame_Time() const
{
	return this->frameTimeMillis;
}

void KeyFrame::Activate( ID3D11DeviceContext* context, unsigned int RegisterSlot ) const
{
	GameAssert( this->frameSRV );
	GameAssert( this->frameData );
	
	context->CSSetShaderResources( RegisterSlot, 1, &this->frameSRV );
}
