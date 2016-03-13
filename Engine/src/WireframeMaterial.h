#pragma once

#include "Material.h"

class WireframeMaterial : public Material
{
public:
	WireframeMaterial();
	virtual ~WireframeMaterial() override;

	WireframeMaterial( ID3D11Device* device );

protected:
	virtual void PrepareBuffers( const DrawInfo& info ) const override;


private:
	void SetupVertexShaderBuffers( ID3D11Device* device );
	void SetupPixelShaderBuffers( ID3D11Device* device );
	void SetupPixelShaderSamplers( ID3D11Device* device );


	WireframeMaterial( const WireframeMaterial& ) = delete;
	WireframeMaterial& operator=( const WireframeMaterial& ) = delete;
};