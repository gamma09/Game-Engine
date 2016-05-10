#pragma once

#include "Material.h"

class UnlitTextureMaterial : public Material
{
public:
	UnlitTextureMaterial();
	virtual ~UnlitTextureMaterial() override;

	UnlitTextureMaterial( ID3D11Device* device );

protected:
	virtual void PrepareBuffers( ID3D11DeviceContext* context, const DirectionLight* ) const override;


private:
	void SetupVertexShaderBuffers( ID3D11Device* device );
	void SetupPixelShaderBuffers( ID3D11Device* device );
	void SetupPixelShaderSamplers( ID3D11Device* device );
	void SetupRasterizer( ID3D11Device* device );


	UnlitTextureMaterial( const UnlitTextureMaterial& ) = delete;
	UnlitTextureMaterial& operator=( const UnlitTextureMaterial& ) = delete;
};