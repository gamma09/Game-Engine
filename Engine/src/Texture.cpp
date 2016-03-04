#include <GameAssert.h>
#include <cstdio>
#include <cstdlib>

#include <GL/glew.h>
#include <SOIL.h>
#include <asset_reader.h>

#include "MemorySetup.h"
#include "Texture.h"
#include "TextureManager.h"


// If something has UV values outside [0, 1], it will be textured bright green
static const float BORDER_COLOR[] = {0.0f, 1.0f, 0.0f, 1.0f};


void Texture::Set(const char* archiveFile, const char* textureName)
{
	unsigned char* bytes;
	int size;
	
	bool status = read_asset(archiveFile, TEXTURE_TYPE, textureName, bytes, size);
	this->using_default_texture = !status;

	if (status)
	{
		int width, height;
		int channels;
		unsigned char* image = SOIL_load_image_from_memory( bytes, size, &width, &height, &channels, SOIL_LOAD_AUTO );
		if ( image )
		{
			this->using_default_texture = true;
			this->texture_obj = TextureManager::Instance()->Default_Texture()->Get_Texture_ID();
		}
		else
		{
			glGenTextures(1, &this->texture_obj);
			glBindTexture(GL_TEXTURE_2D, this->texture_obj);

			// Don't wrap texture if u,v is not [0,1]
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, BORDER_COLOR);

			// Texture filtering - super secret knob here for graphics!
			// (GL_LINEAR better graphics, GL_NEAREST faster)
			// TODO Use Mipmaps!
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			// Interpret file as texture data
			if (channels == 3)
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
			else if (channels == 4)
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
			else
				GameAssert(0); // should not happen, unless tga format works differently than we think
		}
		
		SOIL_free_image_data( image );
		delete[] bytes;
	}
	else
	{
		this->texture_obj = TextureManager::Instance()->Default_Texture()->Get_Texture_ID();
	}
}

void Texture::Reset()
{
	ManagedObject::Reset();

	this->nextTexture = 0;

	if (!this->using_default_texture)
		glDeleteTextures(1, &this->texture_obj);
}

const GLuint Texture::Get_Texture_ID() const
{
	return this->texture_obj;
}


Texture::Texture(const char* defaultArchive, const char* textureName) :
	ManagedObject(),
	using_default_texture(false),
	nextTexture(0),
	texture_obj(0)
{
	unsigned char* bytes;
	int size;
	
	bool status = read_asset(defaultArchive, TEXTURE_TYPE, textureName, bytes, size);
	if (!status)
	{
		// If we're in debug mode, dont exit, let the programmer debug it...
		GameAssert(status);

		// This goes beyond asserts...if we can't load up the default texture, WE'VE GOT BIG PROBLEMS!
		fprintf(stderr, "Error: Could not load default texture \"%s\" from archive \"%s\"!\n", textureName, defaultArchive);
		exit(1);
	}

	glGenTextures(1, &this->texture_obj);
	glBindTexture(GL_TEXTURE_2D, this->texture_obj);

	// Don't wrap texture if u,v is not [0,1]
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, BORDER_COLOR);

	// Texture filtering - super secret knob here for graphics!
	// (GL_LINEAR better graphics, GL_NEAREST faster)
	// TODO Use Mipmaps!
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Interpret file as texture data
	int width, height, channels;
	unsigned char* image = SOIL_load_image_from_memory( bytes, size, &width, &height, &channels, SOIL_LOAD_AUTO );
	GameVerify( image != nullptr );
	delete[] bytes;

	if (channels == 3)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	else if (channels == 4)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	else
		GameAssert(0);

	SOIL_free_image_data( image );
}

Texture::Texture() :
	ManagedObject(),
	texture_obj(0),
	nextTexture(0)
{
	// Do nothing
}

Texture::~Texture()
{
	// Do nothing
}