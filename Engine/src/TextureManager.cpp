#include <assert.h>


#include "TextureManager.h"
#include "Texture.h"
#include "MemorySetup.h"

#define MAX_TEXTURES_CREATED 128


TextureManager* TextureManager::instance;

TextureManager* TextureManager::Instance()
{
	assert(instance != 0);

	return instance;
}
		
void TextureManager::Create(Heap* managerHeap, const uint32_t initialReserve, const uint32_t refillSize)
{
	managerHeap;

	assert(instance == 0);

	instance = new(managerHeap, ALIGN_4) TextureManager(initialReserve, refillSize);
}

void TextureManager::Destroy()
{
	assert(instance != 0);

	instance->Destroy_Objects();
	MemReturnCode status = Mem::destroyHeap(instance->heap);
	status;
	assert(status == Mem_OK);
	delete instance;
	instance = 0;
}

Texture* TextureManager::Add(const char* archiveFile, const char* textureName)
{
	Texture* texture = static_cast<Texture*>(this->Add_Object());
	texture->Set(archiveFile, textureName);
	return texture;
}

const Texture* TextureManager::Default_Texture() const
{
	assert(this->defaultTexture != 0);

	return this->defaultTexture;
}

void TextureManager::Create_Default_Texture()
{
	assert(this->defaultTexture == 0);
	this->defaultTexture = new(this->heap, ALIGN_4) Texture("../resources/Default_Texture.spu", "default");
}


ManagedObject* TextureManager::Make_Object() const
{
	return new(this->heap, ALIGN_4) Texture();
}

void TextureManager::Delete_Object(ManagedObject* obj) const
{
	delete obj;
}


TextureManager::TextureManager(const uint32_t initialReserve, const uint32_t refillSize) :
	Manager(refillSize),
	defaultTexture(0)
{
	MemReturnCode status = Mem::createFixBlockHeap(this->heap, MAX_TEXTURES_CREATED, sizeof(Texture), "Texture Heap");
	status;
	assert(status == Mem_OK);
	this->Init(initialReserve);
}

TextureManager::~TextureManager()
{
	glDeleteTextures(1, &this->defaultTexture->texture_obj);
	delete this->defaultTexture;
}