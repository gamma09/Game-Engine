#pragma once

#include "Material.h"

class LitTextureMaterial : public Material
{
public:
	LitTextureMaterial();
	virtual ~LitTextureMaterial() override;

	LitTextureMaterial( ID3D11Device* device );

protected:
	virtual void PrepareBuffers( const DrawInfo& info ) const override;


private:
	void SetupVertexShaderBuffers( ID3D11Device* device );
	void SetupPixelShaderBuffers( ID3D11Device* device );
	void SetupPixelShaderSamplers( ID3D11Device* device );


	LitTextureMaterial( const LitTextureMaterial& ) = delete;
	LitTextureMaterial& operator=( const LitTextureMaterial& ) = delete;
};