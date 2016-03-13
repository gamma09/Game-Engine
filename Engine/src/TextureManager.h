#pragma once

#include <stdint.h>

#include <Manager.h>

class Texture;
class Heap;
struct ID3D11Device;

class TextureManager : public Manager
{
public:
	static TextureManager* Instance();

	static void Create( Heap* managerHeap, const uint32_t initialReserve, const uint32_t refillSize );
	static void Destroy();

	Texture* Add( ID3D11Device* device, const char* archiveFile, const char* textureName );

	void Create_Default_Texture( ID3D11Device* device );
	const Texture* Default_Texture() const;

protected:
	virtual ManagedObject* Make_Object() const override;
	virtual void Delete_Object( ManagedObject* obj ) const override;

private:
	// creation/destruction of manager can only be done by the manager
	TextureManager( const uint32_t initialReserve, const uint32_t refillSize );
	virtual ~TextureManager();

	// no copying allowed! and no locals either!
	TextureManager();
	TextureManager( const TextureManager& manager );
	TextureManager& operator=( const TextureManager& manager );

private:
	Heap* heap;
	Texture* defaultTexture;

	static TextureManager* instance;
};