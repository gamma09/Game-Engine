#pragma once

#include <stdint.h>
#include <Vect.h>
#include <Quat.h>

struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11Buffer;
struct ID3D11ShaderResourceView;

class KeyFrame
{
public:
	KeyFrame();
	KeyFrame( ID3D11Device* device, uint32_t boneCount, unsigned char*& rawFrameData );
	KeyFrame( KeyFrame&& frame );
	KeyFrame& operator=( KeyFrame&& frame );
	~KeyFrame();

	uint32_t Get_Frame_Time() const;
	void Activate( ID3D11DeviceContext* context, unsigned int registerSlot ) const;

private:
	uint32_t frameTimeMillis;
	ID3D11ShaderResourceView* frameSRV;
	ID3D11Buffer* frameData;



private:
	KeyFrame( const KeyFrame& ) = delete;
	KeyFrame& operator=( const KeyFrame& ) = delete;
	
};