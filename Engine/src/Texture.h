#pragma once

#include <ManagedObject.h>

struct ID3D11Device;
struct ID3D11ShaderResourceView;

class Texture : public ManagedObject
{
public:

	void Set( ID3D11Device* device, const char* archiveFile, const char* textureName );
	virtual void Reset();

	inline ID3D11ShaderResourceView* Get_Texture_Resource() const { return this->textureResource; }



private:
	// Only allow the texture manager to create or destroy a texture
	Texture();
	Texture( ID3D11Device* device, const char* defaultArchive, const char* textureName );
	virtual ~Texture();

	friend class TextureManager;
	friend class ModelBase;

	// Disallow copying
	Texture( const Texture& texture );
	Texture& operator=( const Texture& texture );

private:
	Texture* nextTexture;
	ID3D11ShaderResourceView* textureResource;
	bool using_default_texture;
};