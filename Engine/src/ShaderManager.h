#pragma once

#include <Manager.h>

#include "Shader.h"

class Heap;

class ShaderManager : public Manager
{
public:
	static ShaderManager* Instance();

	static void Create( Heap* managerHeap, uint32_t initialReserve, uint32_t refillSize );
	static void Destroy();

	Shader* Add( const Shader::Name& name, const char* baseFileName, ID3D11Device* device );




protected:
	virtual ManagedObject* Make_Object() const override;
	virtual void Delete_Object( ManagedObject* obj ) const override;


private:
	ShaderManager();
	ShaderManager( uint32_t initialReserve, uint32_t refillSize );
	virtual ~ShaderManager();
	ShaderManager( const ShaderManager& manager );
	ShaderManager& operator=( const ShaderManager& manager );

private:
	Heap* heap;

	static ShaderManager* instance;
};