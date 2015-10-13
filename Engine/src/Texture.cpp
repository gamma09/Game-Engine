#include <cassert>
#include <cstdio>
#include <cstdlib>

#include <GL/gl3w.h>
#include <GL/glfw.h>
#include <eat.h>

#include "MemorySetup.h"
#include "Texture.h"
#include "TextureManager.h"


// If something has UV values outside [0, 1], it will be textured bright green
static const float BORDER_COLOR[] = {0.0f, 1.0f, 0.0f, 1.0f};


void Texture::Set(const char* archiveFile, const char* textureName)
{
	unsigned char* bytes;
	int size;
	
	bool status = eat(archiveFile, TEXTURE_TYPE, textureName, bytes, size);
	this->using_default_texture = !status;

	if (status)
	{
		GLFWimage image;
		int imageReadSuccessfully = glfwReadMemoryImage(bytes, size, &image, 0);
		if (imageReadSuccessfully != GL_TRUE)
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

			int s = sizeof(*image.Data);
			s;

			// Interpret file as texture data
			if (image.BytesPerPixel == 3)
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.Width, image.Height, 0, GL_RGB, GL_UNSIGNED_BYTE, image.Data);
			else if (image.BytesPerPixel == 4)
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.Width, image.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.Data);
			else
				assert(0); // should not happen, unless tga format works differently than we think
		}
		
		glfwFreeImage(&image);
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
	
	bool status = eat(defaultArchive, TEXTURE_TYPE, textureName, bytes, size);
	if (!status)
	{
		// If we're in debug mode, dont exit, let the programmer debug it...
		assert(status);

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
	GLFWimage image;
	int imageReadSuccessfully = glfwReadMemoryImage(bytes, size, &image, 0);
	imageReadSuccessfully;
	assert(imageReadSuccessfully == GL_TRUE);
	delete[] bytes;

	if (image.BytesPerPixel == 3)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.Width, image.Height, 0, GL_RGB, GL_UNSIGNED_BYTE, image.Data);
	else if (image.BytesPerPixel == 4)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.Width, image.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.Data);
	else
		assert(0);

	glfwFreeImage(&image);
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