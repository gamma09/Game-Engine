#include <GameAssert.h>

#include "ShaderManager.h"
#include "MemorySetup.h"





ShaderManager* ShaderManager::instance;


#define MAX_SHADERS_CREATED 4




ShaderManager* ShaderManager::Instance()
{
	GameAssert( instance != 0 );

	return instance;
}

void ShaderManager::Create( Heap* managerHeap, uint32_t initialReserve, uint32_t refillSize )
{
	managerHeap;

	GameAssert( instance == 0 );

	instance = new( managerHeap, ALIGN_4 ) ShaderManager( initialReserve, refillSize );
}

void ShaderManager::Destroy()
{
	GameAssert( instance != 0 );

	instance->Destroy_Objects();
	GameVerify( Mem_OK == Mem::destroyHeap( instance->heap ) );
	delete instance;
	instance = 0;
}

ShaderManager::ShaderManager( uint32_t initialReserve, uint32_t refillSize ) :
Manager( refillSize )
{
	GameVerify( Mem_OK == Mem::createFixBlockHeap( this->heap, MAX_SHADERS_CREATED, sizeof( Shader ) ) );
	this->Init( initialReserve );
}

ShaderManager::~ShaderManager()
{
	// Do nothing
}

Shader* ShaderManager::Add( const Shader::Name& name, const char* baseFileName, ID3D11Device* device )
{
	Shader* shader = static_cast<Shader*>( this->Add_Object() );
	shader->Set( name, baseFileName, device );
	return shader;
}

ManagedObject* ShaderManager::Make_Object() const
{
	return new( this->heap, ALIGN_4 ) Shader();
}

void ShaderManager::Delete_Object( ManagedObject* obj ) const
{
	Shader* shader = static_cast<Shader*>( obj );
	delete shader;
}