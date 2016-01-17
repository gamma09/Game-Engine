#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

class Texture
{
public:
	Texture( const char* textureName, const char* textureFile );
	Texture( const Texture& texture );
	Texture& operator=( const Texture& texture );
	~Texture();

	const char* Get_Texture_Name() const;
	const char* Get_Texture_File() const;

	bool operator==( const Texture& texture ) const;

private:

	Texture();



	char textureName[20];
	char textureFile[MAX_PATH];

};