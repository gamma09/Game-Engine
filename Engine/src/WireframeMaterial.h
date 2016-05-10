#pragma once

#include "Material.h"

// TODO create colorable lines as a geometry shader - see SolidWireframe.zip

class WireframeMaterial : public Material
{
public:
	WireframeMaterial();
	virtual ~WireframeMaterial() override;

	WireframeMaterial( ID3D11Device* device );

protected:
	virtual void PrepareBuffers( ID3D11DeviceContext* context, const DirectionLight* ) const override;


private:
	void SetupVertexShaderBuffers( ID3D11Device* device );
	void SetupPixelShaderBuffers( ID3D11Device* device );
	void SetupPixelShaderSamplers( ID3D11Device* device );
	void SetupRasterizer( ID3D11Device* device );


	WireframeMaterial( const WireframeMaterial& ) = delete;
	WireframeMaterial& operator=( const WireframeMaterial& ) = delete;
};