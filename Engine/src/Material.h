#pragma once

struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11Device;
struct ID3D11InputLayout;
struct ID3D11Buffer;
struct ID3D11DeviceContext;
struct ID3D11SamplerState;
struct DrawInfo;

class Material
{
protected:
	Material();
	Material( const char* shaderFileAsset, ID3D11Device* device );

	virtual void PrepareBuffers( const DrawInfo& info ) const abstract;

public:
	virtual ~Material();

	void Draw( const DrawInfo& info ) const;
	


private:
	Material( const Material& material ) = delete;
	Material& operator=( const Material& material ) = delete;

	void Destroy();
	void SetupInputLayout( ID3D11Device* device, const char* vertexShaderData, unsigned long dataSize );



protected:
	ID3D11InputLayout* inputLayout;

	ID3D11VertexShader* vertexShader;
	ID3D11Buffer** vertexShaderBuffers;
	unsigned int vertexShaderBufferCount;

	ID3D11PixelShader* pixelShader;
	ID3D11Buffer** pixelShaderBuffers;
	unsigned int pixelShaderBufferCount;

	ID3D11SamplerState** pixelShaderSamplers;
	unsigned int pixelShaderSamplerCount;
};