#pragma once

#include <ManagedObject.h>

#include "ReferencedObject.h"

class ShaderManager;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11Device;
struct ID3D11InputLayout;
struct ID3D11Buffer;
struct ID3D11DeviceContext;
struct ID3D11SamplerState;

class Shader : public ManagedObject, public ReferencedObject
{
public:

	enum Name
	{
		TEXTURE_LIGHT,
		TEXTURE,
		WIREFRAME,
		Not_Initialized
	};


public:

	virtual void Set(const Name& name, const char* shaderFileAsset, ID3D11Device* device );
	virtual void Reset();

	void Activate( ID3D11DeviceContext* context ) const;
	ID3D11Buffer* GetVertexShaderBuffer( unsigned int index ) const;
	ID3D11Buffer* GetPixelShaderBuffer( unsigned int index ) const;

protected:
	virtual void Free_Me();
		
private:
	Shader();
	virtual ~Shader();

	void SetupInputLayout( const Name& name, ID3D11Device* device, const char* vertexShaderData, unsigned long dataSize );
	void SetupVertexShaderBuffers( const Name& name, ID3D11Device* device );
	void SetupPixelShaderBuffers( const Name& name, ID3D11Device* device );
	void SetupPixelShaderSamplers( const Name& name, ID3D11Device* device );


	Shader(const Shader& shader) = delete;
	Shader& operator=(const Shader& shader) = delete;
	

private:
	Name name;

	ID3D11InputLayout* inputLayout;

	ID3D11VertexShader* vertexShader;
	ID3D11Buffer** vertexShaderBuffers;
	unsigned int vertexShaderBufferCount;

	ID3D11PixelShader* pixelShader;
	ID3D11Buffer** pixelShaderBuffers;
	unsigned int pixelShaderBufferCount;

	ID3D11SamplerState** pixelShaderSamplers;
	unsigned int pixelShaderSamplerCount;
	
	friend class ShaderManager;
};