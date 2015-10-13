#pragma once

#include <ManagedObject.h>
#include <GL/gl3w.h>

class Texture : public ManagedObject
{
public:
	
	void Set(const char* archiveFile, const char* textureName);
	virtual void Reset();

	const GLuint Get_Texture_ID() const;



private:
	// Only allow the texture manager to create or destroy a texture
	Texture();
	Texture(const char* defaultArchive, const char* textureName);
	virtual ~Texture();

	friend class TextureManager;
	friend class ModelBase;

	// Disallow copying
	Texture(const Texture& texture);
	Texture& operator=(const Texture& texture);

private:
	Texture* nextTexture;
	GLuint texture_obj;
	bool using_default_texture;
};