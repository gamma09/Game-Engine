#include "Texture.h"

Texture::Texture( const char* textureName, const char* textureFile )
{
	strcpy_s( this->textureName, textureName );
	for( int i = strlen( this->textureName ) - 1; i >= 0; i-- )
	{
		if( this->textureName[i] >= 'A' && this->textureName[i] <= 'Z' )
			this->textureName[i] += 'a' - 'A';
	}

	strcpy_s( this->textureFile, textureFile );
}

Texture::Texture( const Texture& texture )
{
	strcpy_s( this->textureName, texture.textureName );
	strcpy_s( this->textureFile, texture.textureFile );
}

Texture& Texture::operator=( const Texture& texture )
{
	strcpy_s( this->textureName, texture.textureName );
	strcpy_s( this->textureFile, texture.textureFile );

	return *this;
}

Texture::~Texture()
{
	// Do nothing
}

const char* Texture::Get_Texture_Name() const
{
	return this->textureName;
}

const char* Texture::Get_Texture_File() const
{
	return this->textureFile;
}

bool Texture::operator==( const Texture& texture ) const
{
	return strcmp( this->textureFile, texture.textureFile ) == 0;
}