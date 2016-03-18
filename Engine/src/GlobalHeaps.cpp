#include <GameAssert.h>
#include <MemoryEngine.h>

#include "GlobalHeaps.h"

TemporaryHeap* TemporaryHeap::instance = nullptr;

void TemporaryHeap::Create()
{
	GameAssert( instance == 0 );

	instance = new TemporaryHeap();
}

void TemporaryHeap::Destroy()
{
	GameAssert( instance != 0 );

	delete instance;
	instance = 0;
}

Heap* TemporaryHeap::Instance()
{
	GameAssert( instance != 0 );

	return instance->heap;
}

// We might need A LOT of memory, depending on what is using this heap...
// Throw down with 128 MB first...
#define TEMPORARY_HEAP_SIZE 134217728
// 128 * 1024 * 1024 = 134,217,728

TemporaryHeap::TemporaryHeap()
{

	Mem::createVariableBlockHeap( this->heap, TEMPORARY_HEAP_SIZE );
}

TemporaryHeap::~TemporaryHeap()
{
	Mem::destroyHeap( this->heap );
}

ConstantBufferHeap* ConstantBufferHeap::instance = nullptr;

void ConstantBufferHeap::Create()
{
	GameAssert( instance == nullptr );
	
	instance = new ConstantBufferHeap();
}

void ConstantBufferHeap::Destroy()
{
	GameAssert( instance != nullptr );

	delete instance;
	instance = nullptr;
}

Heap* ConstantBufferHeap::Instance()
{
	GameAssert( instance != nullptr );

	return instance->heap;
}

// Start with 2 pages of memory
#define CONSTANT_BUFFER_HEAP_SIZE 8192

ConstantBufferHeap::ConstantBufferHeap()
{
	Mem::createVariableBlockHeap( this->heap, CONSTANT_BUFFER_HEAP_SIZE );
}

ConstantBufferHeap::~ConstantBufferHeap()
{
	Mem::destroyHeap( this->heap );
}




AnimHeap* AnimHeap::instance = nullptr;

void AnimHeap::Create()
{
	GameAssert( instance == nullptr );

	instance = new AnimHeap();
}

void AnimHeap::Destroy()
{
	GameAssert( instance != nullptr );
	
	delete instance;
	instance = nullptr;
}

Heap* AnimHeap::Instance()
{
	GameAssert( instance != nullptr );

	return instance->heap;
}

// We need to go big with this, so go for 512 MB to begin with
#define ANIM_HEAP_SIZE 512 * 1024 * 1024

AnimHeap::AnimHeap()
{
	Mem::createVariableBlockHeap( this->heap, ANIM_HEAP_SIZE );
}

AnimHeap::~AnimHeap()
{
	Mem::destroyHeap( this->heap );
}




AssetHeap* AssetHeap::instance = nullptr;

void AssetHeap::Create()
{
	GameAssert( instance == nullptr );

	instance = new AssetHeap();
}

void AssetHeap::Destroy()
{
	GameAssert( instance != nullptr );

	delete instance;
	instance = nullptr;
}

Heap* AssetHeap::Instance()
{
	GameAssert( instance != nullptr );

	return instance->heap;
}

// We can keep this one fairly small - just 1 MB should be enough
#define ASSET_HEAP_SIZE 1024 * 1024

AssetHeap::AssetHeap()
{
	Mem::createVariableBlockHeap( this->heap, ASSET_HEAP_SIZE );
}

AssetHeap::~AssetHeap()
{
	Mem::destroyHeap( this->heap );
}