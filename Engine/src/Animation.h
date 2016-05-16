#pragma once

#include <stdint.h>
#include <Matrix.h>

class KeyFrame;

struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11ShaderResourceView;

class Animation
{
public:

	Animation();
	Animation( ID3D11Device* device, uint32_t boneCount, unsigned char*& frameData );
	Animation( Animation&& anim );
	Animation& operator=( Animation&& anim );
	~Animation();

	float Activate_Key_Frame_SRVs( ID3D11DeviceContext* context, uint32_t time ) const;
	uint32_t Get_Animation_Length() const;

private:
	uint32_t keyFrameCount;
	KeyFrame* keyFrames;
	
	
	
	Animation( const Animation& anim ) = delete;
	Animation& operator=( const Animation& anim ) = delete;
};