#pragma once

struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11Device;
struct ID3D11InputLayout;
struct ID3D11Buffer;
struct ID3D11DeviceContext;
struct ID3D11SamplerState;
struct ID3DUserDefinedAnnotation;
struct ID3D11RasterizerState;
class Camera;
class DirectionLight;
class Model;





class Material
{
protected:
	Material();
	Material( const char* shaderFileAsset, ID3D11Device* device );

	virtual void PrepareBuffers( ID3D11DeviceContext* context, const DirectionLight* light ) const abstract;

public:
	virtual ~Material();

	void Draw( Camera* camera, const DirectionLight* light, ID3D11DeviceContext* context, ID3DUserDefinedAnnotation* annotation ) const;
	


private:
	Material( const Material& material ) = delete;
	Material& operator=( const Material& material ) = delete;

	void Destroy();
	void SetupInputLayout( ID3D11Device* device, const char* vertexShaderData, unsigned long dataSize );



protected:
	ID3D11InputLayout* inputLayout;
	ID3D11RasterizerState* rasterizer;

	ID3D11VertexShader* vertexShader;
	ID3D11Buffer** extraVertexShaderBuffers;
	unsigned int vertexShaderBufferCount;

	ID3D11PixelShader* pixelShader;
	ID3D11Buffer** pixelShaderBuffers;
	unsigned int pixelShaderBufferCount;

	ID3D11SamplerState** pixelShaderSamplers;
	unsigned int pixelShaderSamplerCount;
};