#pragma once

class Shader;
struct DrawInfo;
class Heap;

struct ID3D11Buffer;

class LightingTextureMaterial
{
public:
	static void Create_Material( Heap* materialHeap );
	static void Destroy_Material();

	static void Draw( const DrawInfo& info );

private:
	LightingTextureMaterial();
	~LightingTextureMaterial();

	LightingTextureMaterial( const LightingTextureMaterial& material ) = delete;
	LightingTextureMaterial& operator=( const LightingTextureMaterial& material ) = delete;


private:
	Shader* shader;
	ID3D11Buffer* worldMatrixBuffer;
	ID3D11Buffer* viewMatrixBuffer;
	ID3D11Buffer* projectionMatrixBuffer;
	ID3D11Buffer* lightDataBuffer;
	unsigned int textureUnit;

	static LightingTextureMaterial* instance;
};